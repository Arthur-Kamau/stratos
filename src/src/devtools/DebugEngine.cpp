#include "stratos/DebugEngine.h"
#include "stratos/DevToolsServer.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace stratos {

DebugEngine& DebugEngine::instance() {
    static DebugEngine inst;
    return inst;
}

DebugEngine::DebugEngine()
    : enabled_(false), state_(DebugState::Running),
      stepMode_(StepMode::Over), stepDepth_(0),
      nextBreakpointId_(1), nextFrameId_(1),
      currentLine_(0), devtools_(nullptr) {}

DebugEngine::~DebugEngine() {
    // Ensure we're not blocking any threads
    disable();
}

void DebugEngine::enable() {
    enabled_ = true;
    state_ = DebugState::Running;

    // Notify DevTools of all source files
    if (devtools_) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& [file, content] : sourceFiles_) {
            std::ostringstream oss;
            oss << "{\"scriptId\":\"" << file << "\",\"url\":\"" << file << "\"}";
            devtools_->broadcastEvent("Debugger.scriptParsed", oss.str());
        }
    }
}

void DebugEngine::disable() {
    enabled_ = false;
    // Resume if paused to unblock interpreter thread
    if (state_ == DebugState::Paused) {
        state_ = DebugState::Running;
        pauseCV_.notify_all();
    }
}

bool DebugEngine::isEnabled() const {
    return enabled_;
}

// --- Breakpoint Management ---

int DebugEngine::setBreakpoint(const std::string& file, int line, const std::string& condition) {
    std::lock_guard<std::mutex> lock(mutex_);
    Breakpoint bp;
    bp.id = nextBreakpointId_++;
    bp.file = file;
    bp.line = line;
    bp.condition = condition;
    bp.enabled = true;
    bp.hitCount = 0;
    breakpoints_.push_back(bp);

    if (devtools_) {
        std::ostringstream oss;
        oss << "{\"breakpointId\":" << bp.id
            << ",\"location\":{\"file\":\"" << file << "\",\"line\":" << line << "}}";
        devtools_->broadcastEvent("Debugger.breakpointResolved", oss.str());
    }

    return bp.id;
}

bool DebugEngine::removeBreakpoint(int breakpointId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::remove_if(breakpoints_.begin(), breakpoints_.end(),
        [breakpointId](const Breakpoint& bp) { return bp.id == breakpointId; });
    if (it != breakpoints_.end()) {
        breakpoints_.erase(it, breakpoints_.end());
        return true;
    }
    return false;
}

void DebugEngine::toggleBreakpoint(int breakpointId) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& bp : breakpoints_) {
        if (bp.id == breakpointId) {
            bp.enabled = !bp.enabled;
            return;
        }
    }
}

std::vector<Breakpoint> DebugEngine::getBreakpoints() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return breakpoints_;
}

void DebugEngine::clearAllBreakpoints() {
    std::lock_guard<std::mutex> lock(mutex_);
    breakpoints_.clear();
}

// --- Execution Control ---

void DebugEngine::pause() {
    if (!enabled_) return;
    state_ = DebugState::Paused;
}

void DebugEngine::resume() {
    if (!enabled_) return;
    state_ = DebugState::Running;
    pauseCV_.notify_all();
    notifyResumed();
}

void DebugEngine::stepOver() {
    if (!enabled_) return;
    std::lock_guard<std::mutex> lock(mutex_);
    stepMode_ = StepMode::Over;
    stepDepth_ = static_cast<int>(callStack_.size());
    state_ = DebugState::Stepping;
    pauseCV_.notify_all();
}

void DebugEngine::stepInto() {
    if (!enabled_) return;
    std::lock_guard<std::mutex> lock(mutex_);
    stepMode_ = StepMode::Into;
    stepDepth_ = static_cast<int>(callStack_.size());
    state_ = DebugState::Stepping;
    pauseCV_.notify_all();
}

void DebugEngine::stepOut() {
    if (!enabled_) return;
    std::lock_guard<std::mutex> lock(mutex_);
    stepMode_ = StepMode::Out;
    stepDepth_ = static_cast<int>(callStack_.size());
    state_ = DebugState::Stepping;
    pauseCV_.notify_all();
}

// --- Check Break (called by interpreter on each statement) ---

bool DebugEngine::checkBreak(const std::string& file, int line) {
    if (!enabled_) return true;

    // Update current position
    currentFile_ = file;
    currentLine_ = line;

    bool shouldPause = false;
    std::string reason;

    // Check breakpoints
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& bp : breakpoints_) {
            if (!bp.enabled) continue;

            // Normalize file paths for comparison
            std::string normalFile = file;
            std::string normalBpFile = bp.file;

            // Compare by filename only if paths differ
            auto fname = std::filesystem::path(normalFile).filename().string();
            auto bpFname = std::filesystem::path(normalBpFile).filename().string();

            if ((normalFile == normalBpFile || fname == bpFname) && bp.line == line) {
                bp.hitCount++;
                shouldPause = true;
                reason = "breakpoint";
                break;
            }
        }
    }

    // Check explicit pause request
    if (state_ == DebugState::Paused && !shouldPause) {
        shouldPause = true;
        reason = "pause";
    }

    // Check stepping
    if (state_ == DebugState::Stepping && !shouldPause) {
        std::lock_guard<std::mutex> lock(mutex_);
        int currentDepth = static_cast<int>(callStack_.size());

        switch (stepMode_) {
            case StepMode::Into:
                shouldPause = true;
                reason = "step";
                break;
            case StepMode::Over:
                if (currentDepth <= stepDepth_) {
                    shouldPause = true;
                    reason = "step";
                }
                break;
            case StepMode::Out:
                if (currentDepth < stepDepth_) {
                    shouldPause = true;
                    reason = "step";
                }
                break;
        }
    }

    if (shouldPause) {
        state_ = DebugState::Paused;
        notifyPaused(reason);

        // Block until resumed
        std::unique_lock<std::mutex> lock(mutex_);
        pauseCV_.wait(lock, [this] {
            return state_ != DebugState::Paused;
        });
    }

    return true;
}

// --- Call Stack ---

void DebugEngine::pushFrame(const std::string& functionName, const std::string& file, int line) {
    if (!enabled_) return;
    std::lock_guard<std::mutex> lock(mutex_);
    StackFrame frame;
    frame.frameId = nextFrameId_++;
    frame.functionName = functionName;
    frame.file = file;
    frame.line = line;
    callStack_.push_back(frame);
}

void DebugEngine::popFrame() {
    if (!enabled_) return;
    std::lock_guard<std::mutex> lock(mutex_);
    if (!callStack_.empty()) {
        callStack_.pop_back();
    }
}

std::vector<StackFrame> DebugEngine::getCallStack() const {
    std::lock_guard<std::mutex> lock(mutex_);
    // Return in reverse order (most recent first)
    std::vector<StackFrame> result(callStack_.rbegin(), callStack_.rend());

    // Add current position as top frame if we have one
    if (!currentFile_.empty()) {
        StackFrame topFrame;
        topFrame.frameId = 0;
        topFrame.functionName = callStack_.empty() ? "<global>" : callStack_.back().functionName;
        topFrame.file = currentFile_;
        topFrame.line = currentLine_;
        result.insert(result.begin(), topFrame);
    }

    return result;
}

// --- Variable Inspection ---

void DebugEngine::setVariableProvider(VariableProvider provider) {
    std::lock_guard<std::mutex> lock(mutex_);
    variableProvider_ = provider;
}

std::vector<VariableInfo> DebugEngine::getVariables(int frameId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (variableProvider_) {
        return variableProvider_(frameId);
    }
    return {};
}

void DebugEngine::setExpressionEvaluator(ExpressionEvaluator evaluator) {
    std::lock_guard<std::mutex> lock(mutex_);
    expressionEvaluator_ = evaluator;
}

std::string DebugEngine::evaluateExpression(const std::string& expr, int frameId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (expressionEvaluator_) {
        return expressionEvaluator_(expr, frameId);
    }
    return "\"<evaluation not available>\"";
}

// --- Source Files ---

void DebugEngine::addSourceFile(const std::string& file, const std::string& content) {
    std::lock_guard<std::mutex> lock(mutex_);
    sourceFiles_[file] = content;

    if (devtools_ && enabled_) {
        std::ostringstream oss;
        oss << "{\"scriptId\":\"" << file << "\",\"url\":\"" << file << "\"}";
        devtools_->broadcastEvent("Debugger.scriptParsed", oss.str());
    }
}

std::string DebugEngine::getSourceContent(const std::string& file) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = sourceFiles_.find(file);
    if (it != sourceFiles_.end()) return it->second;

    // Try matching by filename
    auto fname = std::filesystem::path(file).filename().string();
    for (const auto& [path, content] : sourceFiles_) {
        if (std::filesystem::path(path).filename().string() == fname) {
            return content;
        }
    }
    return "";
}

std::vector<std::string> DebugEngine::getSourceFiles() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> files;
    files.reserve(sourceFiles_.size());
    for (const auto& [file, _] : sourceFiles_) {
        files.push_back(file);
    }
    return files;
}

// --- DevTools ---

void DebugEngine::setDevToolsServer(DevToolsServer* server) {
    devtools_ = server;
}

DebugState DebugEngine::getState() const {
    return state_;
}

std::string DebugEngine::getCurrentFile() const {
    return currentFile_;
}

int DebugEngine::getCurrentLine() const {
    return currentLine_;
}

// --- Helpers ---

bool DebugEngine::hasBreakpointAt(const std::string& file, int line) const {
    for (const auto& bp : breakpoints_) {
        if (bp.enabled && bp.file == file && bp.line == line) return true;
    }
    return false;
}

void DebugEngine::notifyPaused(const std::string& reason) {
    if (!devtools_) return;

    std::ostringstream oss;
    oss << "{\"reason\":\"" << reason << "\"";
    oss << ",\"file\":\"" << currentFile_ << "\"";
    oss << ",\"line\":" << currentLine_;

    // Include call stack
    auto stack = getCallStack();
    oss << ",\"callFrames\":[";
    for (size_t i = 0; i < stack.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "{\"frameId\":" << stack[i].frameId;
        oss << ",\"functionName\":\"" << stack[i].functionName << "\"";
        oss << ",\"file\":\"" << stack[i].file << "\"";
        oss << ",\"line\":" << stack[i].line << "}";
    }
    oss << "]}";

    devtools_->broadcastEvent("Debugger.paused", oss.str());
}

void DebugEngine::notifyResumed() {
    if (!devtools_) return;
    devtools_->broadcastEvent("Debugger.resumed", "{}");
}

} // namespace stratos
