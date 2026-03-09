#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <future>
#include <any>
#include <optional>

namespace stratos {

// Forward declarations
enum class PromiseState {
    Pending,
    Fulfilled,
    Rejected
};

/**
 * Promise implementation for async/await support
 */
template<typename T>
class Promise {
public:
    using FulfillCallback = std::function<void(const T&)>;
    using RejectCallback = std::function<void(const std::string&)>;
    using Executor = std::function<void(FulfillCallback, RejectCallback)>;

    explicit Promise(Executor executor);
    Promise() : state_(PromiseState::Pending) {}

    // Chain operations
    template<typename R>
    std::shared_ptr<Promise<R>> then(std::function<R(const T&)> onFulfilled);

    std::shared_ptr<Promise<T>> catch_(std::function<void(const std::string&)> onRejected);
    std::shared_ptr<Promise<T>> finally(std::function<void()> onFinally);

    // State queries
    PromiseState getState() const { return state_; }
    bool isPending() const { return state_ == PromiseState::Pending; }
    bool isFulfilled() const { return state_ == PromiseState::Fulfilled; }
    bool isRejected() const { return state_ == PromiseState::Rejected; }

    // Get value (blocking)
    T await();

    // Resolve/Reject
    void resolve(const T& value);
    void reject(const std::string& error);

private:
    PromiseState state_;
    T value_;
    std::string error_;
    std::vector<FulfillCallback> fulfillCallbacks_;
    std::vector<RejectCallback> rejectCallbacks_;
    std::vector<std::function<void()>> finallyCallbacks_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

/**
 * Event loop for async operations
 */
class EventLoop {
public:
    using Task = std::function<void()>;

    static EventLoop& getInstance();

    // Schedule a task
    void schedule(Task task);

    // Run the event loop (blocking)
    void run();

    // Stop the event loop
    void stop();

    // Check if running
    bool isRunning() const { return running_; }

    // Process one iteration of the event loop
    void tick();

private:
    EventLoop() : running_(false), stopRequested_(false) {}

    std::queue<Task> taskQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCV_;
    bool running_;
    bool stopRequested_;
};

/**
 * Async runtime for managing async operations
 */
class AsyncRuntime {
public:
    static AsyncRuntime& getInstance();

    // Create a promise
    template<typename T>
    std::shared_ptr<Promise<T>> createPromise();

    // Resolve value
    template<typename T>
    std::shared_ptr<Promise<T>> resolve(const T& value);

    // Reject with error
    template<typename T>
    std::shared_ptr<Promise<T>> reject(const std::string& error);

    // Promise.all - wait for all promises
    template<typename T>
    std::shared_ptr<Promise<std::vector<T>>> all(const std::vector<std::shared_ptr<Promise<T>>>& promises);

    // Promise.race - wait for first promise
    template<typename T>
    std::shared_ptr<Promise<T>> race(const std::vector<std::shared_ptr<Promise<T>>>& promises);

    // Delay utility
    std::shared_ptr<Promise<void>> delay(int milliseconds);

    // Spawn async task
    template<typename T>
    std::shared_ptr<Promise<T>> spawn(std::function<T()> task);

private:
    AsyncRuntime() = default;
};

/**
 * Concurrent runtime for goroutines and channels
 */
template<typename T>
class Channel {
public:
    explicit Channel(size_t capacity = 0);

    // Send value (blocking if full)
    bool send(const T& value);

    // Receive value (blocking if empty)
    std::optional<T> receive();

    // Try to receive without blocking
    std::pair<std::optional<T>, bool> tryReceive();

    // Close the channel
    void close();

    // Check if closed
    bool isClosed() const;

private:
    std::queue<T> buffer_;
    size_t capacity_;
    bool closed_;
    std::mutex mutex_;
    std::condition_variable sendCV_;
    std::condition_variable receiveCV_;
};

/**
 * Goroutine spawner
 */
class GoroutineRuntime {
public:
    static GoroutineRuntime& getInstance();

    // Spawn a goroutine
    void go(std::function<void()> func);

    // Get number of active goroutines
    size_t activeCount() const;

    // Wait for all goroutines to complete
    void waitAll();

private:
    GoroutineRuntime() = default;
    std::vector<std::thread> threads_;
    std::mutex threadsMutex_;
    std::atomic<size_t> activeCount_{0};
};

/**
 * Mutex for synchronization
 */
class Mutex {
public:
    void lock();
    void unlock();
    bool tryLock();

private:
    std::mutex mutex_;
};

/**
 * WaitGroup for synchronization
 */
class WaitGroup {
public:
    void add(int delta);
    void done();
    void wait();

private:
    std::atomic<int> count_{0};
    std::mutex mutex_;
    std::condition_variable cv_;
};

} // namespace stratos
