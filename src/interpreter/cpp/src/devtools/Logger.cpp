#include "stratos/Logger.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

namespace stratos {

// ============================================================================
// ConsoleSink Implementation
// ============================================================================

ConsoleSink::ConsoleSink(bool useColor) : useColor_(useColor) {}

void ConsoleSink::write(const LogEntry& entry) {
    // Format timestamp
    auto time_t = std::chrono::system_clock::to_time_t(entry.timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        entry.timestamp.time_since_epoch()) % 1000;

    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &time_t);
#else
    localtime_r(&time_t, &tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    // Build log line
    std::string colorCode = useColor_ ? getColorCode(entry.level) : "";
    std::string reset = useColor_ ? resetColor() : "";

    std::cerr << colorCode
              << "[" << oss.str() << "] "
              << std::setw(5) << logLevelToString(entry.level) << " "
              << reset;

    // Source location if available
    if (!entry.source.file.empty()) {
        std::cerr << entry.source.file << ":" << entry.source.line;
        if (!entry.source.function.empty()) {
            std::cerr << " (" << entry.source.function << ")";
        }
        std::cerr << " - ";
    }

    std::cerr << entry.message;

    // Additional data if present
    if (!entry.data.empty()) {
        std::cerr << " {";
        bool first = true;
        for (const auto& [key, value] : entry.data) {
            if (!first) std::cerr << ", ";
            std::cerr << key << ": " << value;
            first = false;
        }
        std::cerr << "}";
    }

    std::cerr << std::endl;
}

std::string ConsoleSink::getColorCode(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG: return "\033[90m";  // Gray
        case LogLevel::INFO:  return "\033[94m";  // Blue
        case LogLevel::WARN:  return "\033[93m";  // Yellow
        case LogLevel::ERROR: return "\033[91m";  // Red
        case LogLevel::FATAL: return "\033[1;91m"; // Bold Red
        default: return "";
    }
}

std::string ConsoleSink::resetColor() const {
    return "\033[0m";
}

// ============================================================================
// BufferSink Implementation
// ============================================================================

BufferSink::BufferSink(size_t maxEntries) : maxEntries_(maxEntries) {
    entries_.reserve(maxEntries_);
}

void BufferSink::write(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);

    entries_.push_back(entry);

    // Keep only most recent entries
    if (entries_.size() > maxEntries_) {
        entries_.erase(entries_.begin(), entries_.begin() + (entries_.size() - maxEntries_));
    }
}

std::vector<LogEntry> BufferSink::getEntries() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return entries_;
}

std::vector<LogEntry> BufferSink::getEntriesSince(std::chrono::system_clock::time_point since) const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<LogEntry> result;
    for (const auto& entry : entries_) {
        if (entry.timestamp >= since) {
            result.push_back(entry);
        }
    }
    return result;
}

void BufferSink::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    entries_.clear();
}

// ============================================================================
// Logger Implementation
// ============================================================================

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : minLevel_(LogLevel::DEBUG) {
    // Create default sinks
    auto consoleSink = std::make_shared<ConsoleSink>(true);
    bufferSink_ = std::make_shared<BufferSink>(10000);

    sinks_.push_back(consoleSink);
    sinks_.push_back(bufferSink_);
}

void Logger::debug(const std::string& message, const SourceLocation& source) {
    log(LogLevel::DEBUG, message, source);
}

void Logger::info(const std::string& message, const SourceLocation& source) {
    log(LogLevel::INFO, message, source);
}

void Logger::warn(const std::string& message, const SourceLocation& source) {
    log(LogLevel::WARN, message, source);
}

void Logger::error(const std::string& message, const SourceLocation& source) {
    log(LogLevel::ERROR, message, source);
}

void Logger::fatal(const std::string& message, const SourceLocation& source) {
    log(LogLevel::FATAL, message, source);
}

void Logger::log(LogLevel level, const std::string& message, const SourceLocation& source,
                 const std::unordered_map<std::string, std::string>& data) {
    // Check minimum level
    if (level < minLevel_) {
        return;
    }

    // Create log entry
    LogEntry entry(level, message, source);
    entry.data = data;

    // Write to all sinks
    writeToSinks(entry);
}

void Logger::setMinLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    minLevel_ = level;
}

LogLevel Logger::getMinLevel() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return minLevel_;
}

void Logger::addSink(std::shared_ptr<LogSink> sink) {
    std::lock_guard<std::mutex> lock(mutex_);
    sinks_.push_back(sink);
}

void Logger::removeSink(std::shared_ptr<LogSink> sink) {
    std::lock_guard<std::mutex> lock(mutex_);
    sinks_.erase(std::remove(sinks_.begin(), sinks_.end(), sink), sinks_.end());
}

void Logger::clearSinks() {
    std::lock_guard<std::mutex> lock(mutex_);
    sinks_.clear();
}

std::shared_ptr<BufferSink> Logger::getBufferSink() const {
    return bufferSink_;
}

void Logger::writeToSinks(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto& sink : sinks_) {
        try {
            sink->write(entry);
        } catch (const std::exception& e) {
            // Sink failed, but don't crash - just skip it
            std::cerr << "Logger sink failed: " << e.what() << std::endl;
        }
    }
}

} // namespace stratos
