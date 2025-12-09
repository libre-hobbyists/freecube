#ifndef FREECUBE_UTIL_LOG_HPP
#define FREECUBE_UTIL_LOG_HPP

#include <iostream>
#include <string_view>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <type_traits>

namespace freecube::util {

    enum class LogLevel {
        FC_TRACE,
        FC_DEBUG,
        FC_INFO,
        FC_WARN,
        FC_ERROR,
        FC_CRITICAL
    };

    constexpr std::string_view log_level_to_string(LogLevel level) {
        switch (level) {
            case LogLevel::FC_TRACE:    return "TRACE";
            case LogLevel::FC_DEBUG:    return "DEBUG";
            case LogLevel::FC_INFO:     return "INFO";
            case LogLevel::FC_WARN:     return "WARN";
            case LogLevel::FC_ERROR:    return "ERROR";
            case LogLevel::FC_CRITICAL: return "CRITICAL";
        }
        return "???";
    }

    namespace Colours {
        constexpr const char *RESET_FC    = "\033[0m";
        constexpr const char *TRACE_FC    = "\033[37m";
        constexpr const char *DEBUG_FC    = "\033[36m";
        constexpr const char *INFO_FC     = "\033[32m";
        constexpr const char *WARN_FC     = "\033[33m";
        constexpr const char *ERROR_FC    = "\033[31m";
        constexpr const char *CRITICAL_FC = "\033[1;31m";
    }

    constexpr const char* log_level_colour(LogLevel level) {
        switch (level) {
            case LogLevel::FC_TRACE:    return Colours::TRACE_FC;
            case LogLevel::FC_DEBUG:    return Colours::DEBUG_FC;
            case LogLevel::FC_INFO:     return Colours::INFO_FC;
            case LogLevel::FC_WARN:     return Colours::WARN_FC;
            case LogLevel::FC_ERROR:    return Colours::ERROR_FC;
            case LogLevel::FC_CRITICAL: return Colours::CRITICAL_FC;
        }
        return Colours::RESET_FC;
    }

    class LogCFG {
    public:
        static LogLevel min_level;
        static bool use_colours;
        static bool use_timestamps;
        static bool show_locations;
    };

    inline LogLevel LogCFG::min_level     = LogLevel::FC_TRACE;
    inline bool LogCFG::use_colours       = true;
    inline bool LogCFG::use_timestamps    = true;
    inline bool LogCFG::show_locations    = false;

    class Logger {
    public:
        template<LogLevel Level, typename... Args>
        static void log(const char* file, int line, Args&&... args) {
            if (Level < LogCFG::min_level)
                return;
            write_log<Level>(file, line, std::forward<Args>(args)...);
        }

    private:
        // Non-integral fallback
        template<typename T>
        static std::enable_if_t<!std::is_integral_v<std::decay_t<T>>, void>
        stream_arg(std::ostringstream& oss, T&& value) {
            oss << std::forward<T>(value);
        }

        // Integral pretty hex
        template<typename T>
        static std::enable_if_t<std::is_integral_v<std::decay_t<T>>, void>
        stream_arg(std::ostringstream& oss, T value) {
            using U = std::make_unsigned_t<std::decay_t<T>>;
            U v = static_cast<U>(value);
            oss << "0x"
                << std::hex << std::uppercase
                << std::setw(sizeof(U) * 2)
                << std::setfill('0')
                << v
                << std::dec;
        }

        template<LogLevel Level, typename... Args>
        static void write_log(const char* file, int line, Args&&... args) {
            std::ostringstream oss;

            if (LogCFG::use_timestamps) {
                auto now = std::chrono::system_clock::now();
                auto time = std::chrono::system_clock::to_time_t(now);
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now.time_since_epoch()) % 1000;
                oss << "[" << std::put_time(std::localtime(&time), "%H:%M:%S")
                    << "." << std::setw(3) << std::setfill('0') << ms.count()
                    << "] ";
            }

            if (LogCFG::use_colours)
                oss << log_level_colour(Level);

            oss << "[" << log_level_to_string(Level) << "]";

            if (LogCFG::use_colours)
                oss << Colours::RESET_FC;

            oss << " ";

            if (LogCFG::show_locations)
                oss << "[" << file << ":" << line << "] ";

            (stream_arg(oss, std::forward<Args>(args)), ...);
            oss << "\n";

            if constexpr (Level >= LogLevel::FC_ERROR) {
                std::cerr << oss.str();
                std::cerr.flush();
            } else {
                std::cout << oss.str();
                std::cout.flush();
            }
        }
    };

} // namespace freecube::util

#define LOG_TRACE(...) \
    ::freecube::util::Logger::log<::freecube::util::LogLevel::FC_TRACE>(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...) \
    ::freecube::util::Logger::log<::freecube::util::LogLevel::FC_DEBUG>(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...) \
    ::freecube::util::Logger::log<::freecube::util::LogLevel::FC_INFO>(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...) \
    ::freecube::util::Logger::log<::freecube::util::LogLevel::FC_WARN>(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) \
    ::freecube::util::Logger::log<::freecube::util::LogLevel::FC_ERROR>(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_CRITICAL(...) \
    ::freecube::util::Logger::log<::freecube::util::LogLevel::FC_CRITICAL>(__FILE__, __LINE__, __VA_ARGS__)

#endif
