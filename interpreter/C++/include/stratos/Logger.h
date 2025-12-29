#ifndef STRATOS_LOGGER_H
#define STRATOS_LOGGER_H

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <functional>
#include <mutex>
#include <unordered_map>

namespace stratos {

// Log levels (ordered by severity)
enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3,
    FATAL = 4
};

// Convert log level to string
inline std::string logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

// Parse string to log level
inline LogLevel stringToLogLevel(const std::string& str) {
    if (str == "DEBUG") return LogLevel::DEBUG;
    if (str == "INFO")  return LogLevel::INFO;
    if (str == "WARN")  return LogLevel::WARN;
    if (str == "ERROR") return LogLevel::ERROR;
    if (str == "FATAL") return LogLevel::FATAL;
    return LogLevel::INFO; // default
}

// Source location for log entries
struct SourceLocation {
    std::string file;
    int line;
    std::string function;

    SourceLocation(const std::string& f = "", int l = 0, const std::string& fn = "")
        : file(f), line(l), function(fn) {}
};

// Structured log entry
struct LogEntry {
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    std::string message;
    SourceLocation source;
    std::unordered_map<std::string, std::string> data; // Additional structured data

    LogEntry(LogLevel lvl, const std::string& msg, const SourceLocation& src = SourceLocation())
        : timestamp(std::chrono::system_clock::now()), level(lvl), message(msg), source(src) {}
};

// Log sink interface - can output to console, file, DevTools, etc.
class LogSink {
public:
    virtual ~LogSink() = default;
    virtual void write(const LogEntry& entry) = 0;
};

// Console sink - outputs to stderr with color
class ConsoleSink : public LogSink {
public:
    ConsoleSink(bool useColor = true);
    void write(const LogEntry& entry) override;

private:
    bool useColor_;
    std::string getColorCode(LogLevel level) const;
    std::string resetColor() const;
};

// Buffer sink - stores logs in memory for DevTools
class BufferSink : public LogSink {
public:
    BufferSink(size_t maxEntries = 10000);
    void write(const LogEntry& entry) override;

    std::vector<LogEntry> getEntries() const;
    std::vector<LogEntry> getEntriesSince(std::chrono::system_clock::time_point since) const;
    void clear();

private:
    mutable std::mutex mutex_;
    std::vector<LogEntry> entries_;
    size_t maxEntries_;
};

// Main Logger class (singleton)
class Logger {
public:
    static Logger& instance();

    // Logging methods
    void debug(const std::string& message, const SourceLocation& source = SourceLocation());
    void info(const std::string& message, const SourceLocation& source = SourceLocation());
    void warn(const std::string& message, const SourceLocation& source = SourceLocation());
    void error(const std::string& message, const SourceLocation& source = SourceLocation());
    void fatal(const std::string& message, const SourceLocation& source = SourceLocation());

    // Log with structured data
    void log(LogLevel level, const std::string& message, const SourceLocation& source = SourceLocation(),
             const std::unordered_map<std::string, std::string>& data = {});

    // Configuration
    void setMinLevel(LogLevel level);
    LogLevel getMinLevel() const;

    void addSink(std::shared_ptr<LogSink> sink);
    void removeSink(std::shared_ptr<LogSink> sink);
    void clearSinks();

    // Get buffer sink for DevTools
    std::shared_ptr<BufferSink> getBufferSink() const;

private:
    Logger();
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void writeToSinks(const LogEntry& entry);

    LogLevel minLevel_;
    std::vector<std::shared_ptr<LogSink>> sinks_;
    std::shared_ptr<BufferSink> bufferSink_;
    mutable std::mutex mutex_;
};

// Convenience macros for logging with automatic source location
#define STRATOS_LOG_DEBUG(msg) \
    stratos::Logger::instance().debug((msg), stratos::SourceLocation(__FILE__, __LINE__, __func__))

#define STRATOS_LOG_INFO(msg) \
    stratos::Logger::instance().info((msg), stratos::SourceLocation(__FILE__, __LINE__, __func__))

#define STRATOS_LOG_WARN(msg) \
    stratos::Logger::instance().warn((msg), stratos::SourceLocation(__FILE__, __LINE__, __func__))

#define STRATOS_LOG_ERROR(msg) \
    stratos::Logger::instance().error((msg), stratos::SourceLocation(__FILE__, __LINE__, __func__))

#define STRATOS_LOG_FATAL(msg) \
    stratos::Logger::instance().fatal((msg), stratos::SourceLocation(__FILE__, __LINE__, __func__))

// Macro for structured logging
#define STRATOS_LOG(level, msg, data) \
    stratos::Logger::instance().log((level), (msg), stratos::SourceLocation(__FILE__, __LINE__, __func__), (data))

} // namespace stratos

#endif // STRATOS_LOGGER_H
