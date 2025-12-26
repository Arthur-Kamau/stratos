#include "stratos/AsyncRuntime.h"
#include <chrono>

namespace stratos {

// ============================================================================
// Promise Implementation
// ============================================================================

template<typename T>
Promise<T>::Promise(Executor executor) : state_(PromiseState::Pending) {
    auto fulfill = [this](const T& value) {
        this->resolve(value);
    };

    auto reject = [this](const std::string& error) {
        this->reject(error);
    };

    // Execute asynchronously
    EventLoop::getInstance().schedule([executor, fulfill, reject]() {
        executor(fulfill, reject);
    });
}

template<typename T>
template<typename R>
std::shared_ptr<Promise<R>> Promise<T>::then(std::function<R(const T&)> onFulfilled) {
    auto newPromise = std::make_shared<Promise<R>>();

    std::lock_guard<std::mutex> lock(mutex_);

    if (state_ == PromiseState::Fulfilled) {
        // Already fulfilled, execute immediately
        EventLoop::getInstance().schedule([newPromise, onFulfilled, this]() {
            try {
                R result = onFulfilled(this->value_);
                newPromise->resolve(result);
            } catch (const std::exception& e) {
                newPromise->reject(e.what());
            }
        });
    } else if (state_ == PromiseState::Pending) {
        // Not yet fulfilled, queue the callback
        fulfillCallbacks_.push_back([newPromise, onFulfilled](const T& value) {
            try {
                R result = onFulfilled(value);
                newPromise->resolve(result);
            } catch (const std::exception& e) {
                newPromise->reject(e.what());
            }
        });
    }
    // If rejected, don't call then callback

    return newPromise;
}

template<typename T>
std::shared_ptr<Promise<T>> Promise<T>::catch_(std::function<void(const std::string&)> onRejected) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (state_ == PromiseState::Rejected) {
        EventLoop::getInstance().schedule([onRejected, this]() {
            onRejected(this->error_);
        });
    } else if (state_ == PromiseState::Pending) {
        rejectCallbacks_.push_back(onRejected);
    }

    return std::make_shared<Promise<T>>(*this);
}

template<typename T>
std::shared_ptr<Promise<T>> Promise<T>::finally(std::function<void()> onFinally) {
    std::lock_guard<std::mutex> lock(mutex_);
    finallyCallbacks_.push_back(onFinally);
    return std::make_shared<Promise<T>>(*this);
}

template<typename T>
T Promise<T>::await() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this]() {
        return state_ != PromiseState::Pending;
    });

    if (state_ == PromiseState::Fulfilled) {
        return value_;
    } else {
        throw std::runtime_error("Promise rejected: " + error_);
    }
}

template<typename T>
void Promise<T>::resolve(const T& value) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (state_ != PromiseState::Pending) {
        return; // Already resolved or rejected
    }

    state_ = PromiseState::Fulfilled;
    value_ = value;

    // Execute fulfill callbacks
    for (auto& callback : fulfillCallbacks_) {
        EventLoop::getInstance().schedule([callback, value]() {
            callback(value);
        });
    }
    fulfillCallbacks_.clear();

    // Execute finally callbacks
    for (auto& callback : finallyCallbacks_) {
        EventLoop::getInstance().schedule(callback);
    }
    finallyCallbacks_.clear();

    cv_.notify_all();
}

template<typename T>
void Promise<T>::reject(const std::string& error) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (state_ != PromiseState::Pending) {
        return; // Already resolved or rejected
    }

    state_ = PromiseState::Rejected;
    error_ = error;

    // Execute reject callbacks
    for (auto& callback : rejectCallbacks_) {
        EventLoop::getInstance().schedule([callback, error]() {
            callback(error);
        });
    }
    rejectCallbacks_.clear();

    // Execute finally callbacks
    for (auto& callback : finallyCallbacks_) {
        EventLoop::getInstance().schedule(callback);
    }
    finallyCallbacks_.clear();

    cv_.notify_all();
}

// ============================================================================
// EventLoop Implementation
// ============================================================================

EventLoop& EventLoop::getInstance() {
    static EventLoop instance;
    return instance;
}

void EventLoop::schedule(Task task) {
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        taskQueue_.push(task);
    }
    queueCV_.notify_one();
}

void EventLoop::run() {
    running_ = true;
    stopRequested_ = false;

    while (!stopRequested_) {
        tick();
    }

    running_ = false;
}

void EventLoop::stop() {
    stopRequested_ = true;
    queueCV_.notify_all();
}

void EventLoop::tick() {
    Task task;

    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        queueCV_.wait_for(lock, std::chrono::milliseconds(10), [this]() {
            return !taskQueue_.empty() || stopRequested_;
        });

        if (taskQueue_.empty()) {
            return;
        }

        task = std::move(taskQueue_.front());
        taskQueue_.pop();
    }

    // Execute task outside of lock
    if (task) {
        task();
    }
}

// ============================================================================
// AsyncRuntime Implementation
// ============================================================================

AsyncRuntime& AsyncRuntime::getInstance() {
    static AsyncRuntime instance;
    return instance;
}

template<typename T>
std::shared_ptr<Promise<T>> AsyncRuntime::createPromise() {
    return std::make_shared<Promise<T>>();
}

template<typename T>
std::shared_ptr<Promise<T>> AsyncRuntime::resolve(const T& value) {
    auto promise = std::make_shared<Promise<T>>();
    promise->resolve(value);
    return promise;
}

template<typename T>
std::shared_ptr<Promise<T>> AsyncRuntime::reject(const std::string& error) {
    auto promise = std::make_shared<Promise<T>>();
    promise->reject(error);
    return promise;
}

template<typename T>
std::shared_ptr<Promise<std::vector<T>>> AsyncRuntime::all(
    const std::vector<std::shared_ptr<Promise<T>>>& promises) {

    auto allPromise = std::make_shared<Promise<std::vector<T>>>();

    // Use shared state to track completion
    struct State {
        std::mutex mutex;
        std::vector<T> results;
        size_t completed = 0;
        bool rejected = false;
    };
    auto state = std::make_shared<State>();
    state->results.resize(promises.size());

    for (size_t i = 0; i < promises.size(); i++) {
        promises[i]->then([allPromise, state, i, total = promises.size()](const T& value) -> void {
            std::lock_guard<std::mutex> lock(state->mutex);

            if (state->rejected) return;

            state->results[i] = value;
            state->completed++;

            if (state->completed == total) {
                allPromise->resolve(state->results);
            }
        });

        promises[i]->catch_([allPromise, state](const std::string& error) {
            std::lock_guard<std::mutex> lock(state->mutex);

            if (!state->rejected) {
                state->rejected = true;
                allPromise->reject(error);
            }
        });
    }

    return allPromise;
}

template<typename T>
std::shared_ptr<Promise<T>> AsyncRuntime::race(
    const std::vector<std::shared_ptr<Promise<T>>>& promises) {

    auto racePromise = std::make_shared<Promise<T>>();

    struct State {
        std::mutex mutex;
        bool settled = false;
    };
    auto state = std::make_shared<State>();

    for (auto& promise : promises) {
        promise->then([racePromise, state](const T& value) -> void {
            std::lock_guard<std::mutex> lock(state->mutex);
            if (!state->settled) {
                state->settled = true;
                racePromise->resolve(value);
            }
        });

        promise->catch_([racePromise, state](const std::string& error) {
            std::lock_guard<std::mutex> lock(state->mutex);
            if (!state->settled) {
                state->settled = true;
                racePromise->reject(error);
            }
        });
    }

    return racePromise;
}

std::shared_ptr<Promise<void>> AsyncRuntime::delay(int milliseconds) {
    auto promise = std::make_shared<Promise<void>>();

    std::thread([promise, milliseconds]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        promise->resolve();
    }).detach();

    return promise;
}

template<typename T>
std::shared_ptr<Promise<T>> AsyncRuntime::spawn(std::function<T()> task) {
    auto promise = std::make_shared<Promise<T>>();

    std::thread([promise, task]() {
        try {
            T result = task();
            promise->resolve(result);
        } catch (const std::exception& e) {
            promise->reject(e.what());
        }
    }).detach();

    return promise;
}

// ============================================================================
// Channel Implementation
// ============================================================================

template<typename T>
Channel<T>::Channel(size_t capacity)
    : capacity_(capacity), closed_(false) {}

template<typename T>
bool Channel<T>::send(const T& value) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (closed_) {
        return false;
    }

    // Wait if channel is full
    if (capacity_ > 0) {
        sendCV_.wait(lock, [this]() {
            return buffer_.size() < capacity_ || closed_;
        });

        if (closed_) {
            return false;
        }
    }

    buffer_.push(value);
    receiveCV_.notify_one();
    return true;
}

template<typename T>
std::optional<T> Channel<T>::receive() {
    std::unique_lock<std::mutex> lock(mutex_);

    // Wait for data
    receiveCV_.wait(lock, [this]() {
        return !buffer_.empty() || closed_;
    });

    if (buffer_.empty()) {
        return std::nullopt; // Channel closed and empty
    }

    T value = buffer_.front();
    buffer_.pop();

    sendCV_.notify_one(); // Notify sender if waiting

    return value;
}

template<typename T>
std::pair<std::optional<T>, bool> Channel<T>::tryReceive() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (buffer_.empty()) {
        return {std::nullopt, false};
    }

    T value = buffer_.front();
    buffer_.pop();

    sendCV_.notify_one();

    return {value, true};
}

template<typename T>
void Channel<T>::close() {
    std::lock_guard<std::mutex> lock(mutex_);
    closed_ = true;
    sendCV_.notify_all();
    receiveCV_.notify_all();
}

template<typename T>
bool Channel<T>::isClosed() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return closed_;
}

// ============================================================================
// GoroutineRuntime Implementation
// ============================================================================

GoroutineRuntime& GoroutineRuntime::getInstance() {
    static GoroutineRuntime instance;
    return instance;
}

void GoroutineRuntime::go(std::function<void()> func) {
    activeCount_++;

    std::thread([this, func]() {
        func();
        activeCount_--;
    }).detach();
}

size_t GoroutineRuntime::activeCount() const {
    return activeCount_.load();
}

void GoroutineRuntime::waitAll() {
    while (activeCount_.load() > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

// ============================================================================
// Mutex Implementation
// ============================================================================

void Mutex::lock() {
    mutex_.lock();
}

void Mutex::unlock() {
    mutex_.unlock();
}

bool Mutex::tryLock() {
    return mutex_.try_lock();
}

// ============================================================================
// WaitGroup Implementation
// ============================================================================

void WaitGroup::add(int delta) {
    count_ += delta;
}

void WaitGroup::done() {
    count_--;
    if (count_ == 0) {
        cv_.notify_all();
    }
}

void WaitGroup::wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this]() {
        return count_ == 0;
    });
}

// Explicit template instantiations for common types
template class Promise<int>;
template class Promise<double>;
template class Promise<std::string>;
template class Promise<void>;
template class Channel<int>;
template class Channel<double>;
template class Channel<std::string>;

} // namespace stratos
