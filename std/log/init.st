// Log module - Structured logging
// Inspired by Go's slog and structured logging libraries

package log;

use time;
use strings;

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

    constructor(level: LogLevel, format: LogFormat, fields: Map<string, any>) {
        this.level = level;
        this.format = format;
        this.fields = fields;
    }

    fn debug(message: string, fields: Map<string, any>) void {
        debugWith(message, fields);
    }
    
    fn info(message: string, fields: Map<string, any>) void {
        infoWith(message, fields);
    }
    
    fn warn(message: string, fields: Map<string, any>) void {
        warnWith(message, fields);
    }
    
    fn error(message: string, fields: Map<string, any>) void {
        errorWith(message, fields);
    }
    
    fn fatal(message: string, fields: Map<string, any>) void {
        fatalWith(message, fields);
    }

    fn with(fields: Map<string, any>) Logger {
        // TODO: Merge fields
        return Logger(this.level, this.format, fields);
    }
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

fn setLevel(level: LogLevel) void;
fn setFormat(format: LogFormat) void;

// Output configuration
// fn setOutput(writer: Writer) void; // Writer not defined
fn setFile(path: string) Result<void, Error>; 

// ============================================================================
// Pure Stratos implementations
// ============================================================================

// Logger creation
fn newLogger(level: LogLevel, format: LogFormat) Logger {
    // Assuming Map constructor or literal is available, or null is valid
    // Using a placeholder empty map or null. 
    // Since we don't import collections, we can't use newMap().
    // We assume Map is a built-in type that can be instantiated or we pass null.
    return Logger(level, format, None);
}

// Structured logging wrappers
fn debugWith(message: string, fields: Map<string, any>) void {
    debug(message + formatFields(fields));
}

fn infoWith(message: string, fields: Map<string, any>) void {
    info(message + formatFields(fields));
}

fn warnWith(message: string, fields: Map<string, any>) void {
    warn(message + formatFields(fields));
}

fn errorWith(message: string, fields: Map<string, any>) void {
    error(message + formatFields(fields));
}

fn fatalWith(message: string, fields: Map<string, any>) void {
    fatal(message + formatFields(fields));
}

fn formatFields(fields: Map<string, any>) string {
    if (fields.lenth == 0) {
        return "";
    }
    // Placeholder for field formatting
    return " [fields]";
}

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

// LogTimer for performance logging
class LogTimer {
    var name: string;
    var startTime: int;

    constructor(timerName: string) {
        this.name = timerName;
        // time.now() returns Time object in std/time definition.
        // We assume std/time matches the native implementation or runtime behavior.
        this.startTime = time.now().unix();
    }

    fn elapsed() int {
        return time.now().unix() - this.startTime;
    }

    fn log() void {
        val elapsed = this.elapsed();
        info("LogTimer completed: " + this.name + " elapsed_ms: " + elapsed);
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
        timer.log();
        return result;
    };
}