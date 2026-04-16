#ifndef LOG_UTIL_H
#define LOG_UTIL_H

#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>

class LogStream {
   public:
    LogStream(std::ostream& stream, std::mutex& mutex)
        : stream_(stream), mutex_(mutex) {}

    ~LogStream() {
        if (moved_) return;
        std::lock_guard<std::mutex> lock(mutex_);
        prefix();
        stream_ << str_.str() << std::endl;
        stream_.flush();
    }

    LogStream(const LogStream&) = delete;
    LogStream& operator=(const LogStream&) = delete;

    LogStream(LogStream&& other) noexcept
        : stream_(other.stream_),
          mutex_(other.mutex_),
          str_(std::move(other.str_)) {
        other.moved_ = true;
    }

    template <typename T>
    LogStream& operator<<(const T& val) {
        str_ << val;
        return *this;
    }

    void prefix() {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        stream_ << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S ");
    }

   private:
    std::ostream& stream_;
    std::mutex& mutex_;
    std::stringstream str_;
    bool moved_ = false;
};

class Logger {
   public:
    static Logger& instance() {
        static Logger instance;
        return instance;
    }

    void init(std::ostream* stream) { stream_ = stream; }

    LogStream stream() {
        return LogStream(stream_ ? *stream_ : std::cout, mutex_);
    }

    ~Logger() {
        if (stream_) {
            delete stream_;
            stream_ = nullptr;
        }
    }

   private:
    std::ostream* stream_ = nullptr;
    std::mutex mutex_;
};

#define LOG_DBG_LEVEL 0
#define LOG_INF_LEVEL 1
#define LOG_ERR_LEVEL 2

extern int log_level;

#define LOG_DBG                                                       \
    if (LOG_DBG_LEVEL >= log_level)                                   \
    Logger::instance().stream() << "[DEBUG][" << __FUNCTION__ << "][" \
                                << __FILE__ << ":" << __LINE__ << "]"
#define LOG_INF                                                      \
    if (LOG_INF_LEVEL >= log_level)                                  \
    Logger::instance().stream() << "[INFO][" << __FUNCTION__ << "][" \
                                << __FILE__ << ":" << __LINE__ << "]"
#define LOG_ERR                                                       \
    if (LOG_ERR_LEVEL >= log_level)                                   \
    Logger::instance().stream() << "[ERROR][" << __FUNCTION__ << "][" \
                                << __FILE__ << ":" << __LINE__ << "]"

void log_init();
#endif
