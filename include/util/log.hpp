/**
 * @file log.hpp
 * @brief Logging system for freecube
 * 
 * Provides compile-time configurable logging with multiple severity levels.
 * This header is self contained and does not have a CXX implementation file.
 */

#ifndef FREECUBE_UTIL_LOG_HPP
#define FREECUBE_UTIL_LOG_HPP

#include <iostream>
#include <string_view>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace freecube::util {

    enum class LogLevel {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        CRITICAL
    };

    /**
     * @brief Converts LogLevel enum to string repr
     */
    constexpr std::string_view log_level_to_string(LogLevel level) {
        switch (level) {
            case LogLevel::TRACE:    return "TRACE";
            case LogLevel::DEBUG:    return "DEBUG";
            case LogLevel::INFO:     return "INFO";
            case LogLevel::WARN:     return "WARN";
            case LogLevel::ERROR:    return "ERROR";
            case LogLevel::CRITICAL: return "CRITICAL";
        }
        return "???";
    }

    namespace Colours {
        constexpr const char *RESET    = "\033[0m";      // Reset to defaults
        constexpr const char *TRACE    = "\033[37m";     // White
        constexpr const char *DEBUG    = "\033[36m";     // Cyan
        constexpr const char *INFO     = "\033[32m";     // Green
        constexpr const char *WARN     = "\033[33m";     // Yellow
        constexpr const char *ERROR    = "\033[31m";     // Red
        constexpr const char *CRITICAL = "\033[1;31m";   // Bold Red
    }

    /**
     * @brief Get colour code for log level
     */
    constexpr const char* log_level_colour(LogLevel level) {
        switch (level) {
            case LogLevel::TRACE:    return Colours::TRACE;
            case LogLevel::DEBUG:    return Colours::DEBUG;
            case LogLevel::INFO:     return Colours::INFO;
            case LogLevel::WARN:     return Colours::WARN;
            case LogLevel::ERROR:    return Colours::ERROR;
            case LogLevel::CRITICAL: return Colours::CRITICAL;
        }
        return Colours::RESET;
    }

    /**
     * @brief Log Configuration subclass
     */
    class LogCFG {
    public:
        static LogLevel min_level;
        static bool use_colours;
        static bool use_timestamps;
        static bool show_locations;
    };

    // Default configuration
    inline LogLevel LogCFG::min_level = LogLevel::INFO;
    inline bool LogCFG::use_colours = true;
    inline bool LogCFG::use_timestamps = true;
    inline bool LogCFG::show_locations = false;

    /**
     * @brief Core log implementation
     */
    class Logger {
    public:
        /**
         * @brief Logs message at specific level
         * 
         * @tparam Level Compile-time log level
         * @tparam Args Variadic template for message args
         * @param file Source file name
         * @param line Line number
         * @param args Message components to log
         */
        template<LogLevel Level, typename... Args>
        static void log(const char* file, int line, Args&&... args) {
            if constexpr (Level >= LogLevel::INFO) {
                if (Level < LogCFG::min_level) {
                    return;
                }

                write_log<Level>(file, line, std::forward<Args>(args)...);
            }
        }

    private:
        template<LogLevel Level, typename... Args>
        static void write_log(const char* file, int line, Args&&... args) {
            std::ostringstream oss;

            // Get timestamp
            if (LogCFG::use_timestamps) {
                auto now = std::chrono::system_clock::now();
                auto time = std::chrono::system_clock::to_time_t(now);
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now.time_since_epoch()
                ) % 1000;

                oss << "[" << std::put_time(std::localtime(&time), "%H:%M:%S")
                    << "." << std::setfill('0') << std::setw(3) << ms.count() << "] ";
            }

            // Colour and level
            if (LogCFG::use_colours) {
                oss << log_level_colour(Level);
            }
            oss << "[" << log_level_to_string(Level) << "]";
            if (LogCFG::use_colours) {
                oss << Colours::RESET;
            }
            oss << " ";

            // Source location
            if (LogCFG::show_locations) {
                oss << "[" << file << ":" << line << "] ";
            }

            // Message
            ((oss << std::forward<Args>(args)), ...);
            oss << "\n";

            // Output to appropriate stream
            if constexpr (Level >= LogLevel::ERROR) {
                std::cerr << oss.str();
                std::cerr.flush();
            } else {
                std::cout << oss.str();
                std::cout.flush();
            }
        }
    };

}   // namespace freecube::util

// Convenience macros for logging
#define LOG_TRACE(...) \
    ::freecube::util::Logger::log<::freecube::util::LogLevel::TRACE>( \
        __FILE__, __LINE__, __VA_ARGS__)

#define LOG_DEBUG(...) \
    ::freecube::util::Logger::log<::freecube::util::LogLevel::DEBUG>( \
        __FILE__, __LINE__, __VA_ARGS__)

#define LOG_INFO(...) \
    ::freecube::util::Logger::log<::freecube::util::LogLevel::INFO>( \
        __FILE__, __LINE__, __VA_ARGS__)

#define LOG_WARN(...) \
    ::freecube::util::Logger::log<::freecube::util::LogLevel::WARN>( \
        __FILE__, __LINE__, __VA_ARGS__)

#define LOG_ERROR(...) \
    ::freecube::util::Logger::log<::freecube::util::LogLevel::ERROR>( \
        __FILE__, __LINE__, __VA_ARGS__)

#define LOG_CRITICAL(...) \
    ::freecube::util::Logger::log<::freecube::util::LogLevel::CRITICAL>( \
        __FILE__, __LINE__, __VA_ARGS__)

#endif