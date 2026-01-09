// Log module - Structured logging
// Inspired by Go's slog and structured logging libraries

package log;

use time;
use strings;
use io;

// ============================================================================
// Types
// ============================================================================

enum LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
}

enum LogFormat {
    TEXT,
    JSON
}

class Logger {
    var level: LogLevel;
    var format: LogFormat;
    var fields: Map<string, any>;

    fn debug(message: string, fields: Map<string, any>) void;
    fn info(message: string, fields: Map<string, any>) void;
    fn warn(message: string, fields: Map<string, any>) void;
    fn error(message: string, fields: Map<string, any>) void;
    fn fatal(message: string, fields: Map<string, any>) void;

    fn with(fields: Map<string, any>) Logger;
}

// ============================================================================
// Native function declarations
// ============================================================================

// Global logging functions
fn debug(message: string) void;
fn info(message: string) void;
fn warn(message: string) void;
fn error(message: string) void;
fn fatal(message: string) void;

// Structured logging
fn debugWith(message: string, fields: Map<string, any>) void;
fn infoWith(message: string, fields: Map<string, any>) void;
fn warnWith(message: string, fields: Map<string, any>) void;
fn errorWith(message: string, fields: Map<string, any>) void;
fn fatalWith(message: string, fields: Map<string, any>) void;

// Logger creation
fn newLogger(level: LogLevel, format: LogFormat) Logger {
    return Logger {
        level: level,
        format: format,
        fields: {}
    };
}
fn setLevel(level: LogLevel) void;
fn setFormat(format: LogFormat) void;

// Output configuration
fn setOutput(writer: Writer) void;
fn setFile(path: string) Result<void, Error>;

// ============================================================================
// Pure Stratos implementations
// ============================================================================

// Default logger instance
var defaultLogger = newLogger(LogLevel.INFO, LogFormat.TEXT);

// Convenient logging with format
fn infof(format: string, args: Array<any>) void {
    val message = sprintf(format, args);
    info(message);
}

fn debugf(format: string, args: Array<any>) void {
    val message = sprintf(format, args);
    debug(message);
}

fn warnf(format: string, args: Array<any>) void {
    val message = sprintf(format, args);
    warn(message);
}

fn errorf(format: string, args: Array<any>) void {
    val message = sprintf(format, args);
    error(message);
}

// Create logger with context
fn withContext(fields: Map<string, any>) Logger {
    return defaultLogger.with(fields);
}

// LogTimer for performance logging (renamed from Timer to avoid conflict with time.Timer)
class LogTimer {
    var name: string;
    var startTime: int;

    constructor(timerName: string) {
        this.name = timerName;
        this.startTime = time.now().unix();
    }

    fn elapsed() int {
        return time.now().unix() - this.startTime;
    }

    fn printLog() void {
        val elapsed = this.elapsed();
        infoWith("LogTimer completed", {
            name: this.name,
            elapsed_ms: elapsed
        });
    }
}

fn startTimer(name: string) LogTimer {
    return LogTimer(name);
}

// Log middleware for functions
fn logFunction(name: string, callback: Function) any {
    return () => {
        info("Function called: " + name);
        val timer = startTimer(name);
        val result = callback();
        timer.printLog();
        return result;
    };
}
