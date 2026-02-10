#ifndef STRATOS_DEBUG_ENGINE_H
#define STRATOS_DEBUG_ENGINE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>

namespace stratos {

// Forward declarations
class DevToolsServer;
struct RuntimeValue;

// Debug execution state
enum class DebugState {
    Running,
    Paused,
    Stepping
};

// Step mode when stepping
enum class StepMode {
    Over,
    Into,
    Out
};

// Breakpoint definition
struct Breakpoint {
    int id;
    std::string file;
    int line;
    std::string condition; // Optional conditional expression
    bool enabled = true;
    int hitCount = 0;
};

// Call stack frame
struct StackFrame {
    int frameId;
    std::string functionName;
    std::string file;
    int line;
    // Variable names visible in this frame (populated on demand)
};

// Variable info for inspection
struct VariableInfo {
    std::string name;
    std::string value;
    std::string type;
};

// Debug Engine - manages breakpoints, stepping, and program state inspection
class DebugEngine {
public:
    // Singleton access
    static DebugEngine& instance();

    DebugEngine();
    ~DebugEngine();

    // Enable/disable debugging
    void enable();
    void disable();
    bool isEnabled() const;

    // Breakpoint management
    int setBreakpoint(const std::string& file, int line, const std::string& condition = "");
    bool removeBreakpoint(int breakpointId);
    void toggleBreakpoint(int breakpointId);
    std::vector<Breakpoint> getBreakpoints() const;
    void clearAllBreakpoints();

    // Execution control
    void pause();
    void resume();
    void stepOver();
    void stepInto();
    void stepOut();

    // Called by the interpreter before executing each statement
    // Returns true if execution should continue, false if paused
    bool checkBreak(const std::string& file, int line);

    // Call stack management (called by Interpreter)
    void pushFrame(const std::string& functionName, const std::string& file, int line);
    void popFrame();
    std::vector<StackFrame> getCallStack() const;

    // Variable inspection callback
    // Set by the Interpreter to provide variable data from Environment
    using VariableProvider = std::function<std::vector<VariableInfo>(int frameDepth)>;
    void setVariableProvider(VariableProvider provider);
    std::vector<VariableInfo> getVariables(int frameId = 0) const;

    // Expression evaluation callback
    using ExpressionEvaluator = std::function<std::string(const std::string& expr, int frameDepth)>;
    void setExpressionEvaluator(ExpressionEvaluator evaluator);
    std::string evaluateExpression(const std::string& expr, int frameId = 0) const;

    // Source file tracking
    void addSourceFile(const std::string& file, const std::string& content);
    std::string getSourceContent(const std::string& file) const;
    std::vector<std::string> getSourceFiles() const;

    // DevTools integration
    void setDevToolsServer(DevToolsServer* server);

    // Get current state
    DebugState getState() const;
    std::string getCurrentFile() const;
    int getCurrentLine() const;

private:
    std::atomic<bool> enabled_;
    std::atomic<DebugState> state_;
    StepMode stepMode_;
    int stepDepth_; // call stack depth when step was initiated

    mutable std::mutex mutex_;
    std::condition_variable pauseCV_;

    // Breakpoints
    int nextBreakpointId_;
    std::vector<Breakpoint> breakpoints_;

    // Call stack
    int nextFrameId_;
    std::vector<StackFrame> callStack_;

    // Current position
    std::string currentFile_;
    int currentLine_;

    // Source files
    std::unordered_map<std::string, std::string> sourceFiles_;

    // Callbacks
    VariableProvider variableProvider_;
    ExpressionEvaluator expressionEvaluator_;

    // DevTools server
    DevToolsServer* devtools_;

    // Helpers
    bool hasBreakpointAt(const std::string& file, int line) const;
    void notifyPaused(const std::string& reason);
    void notifyResumed();
};

} // namespace stratos

#endif // STRATOS_DEBUG_ENGINE_H
