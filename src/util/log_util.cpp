//
// Created by ed on 8/11/25.
//

#include "log_util.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <nlohmann/json.hpp>

static void log_plain(const std::string &level, const std::string &message) {
    using namespace std::chrono;

    // Get current time
    const auto now = system_clock::now();
    const auto now_time_t = system_clock::to_time_t(now);
    const auto now_ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    // Format timestamp (YYYY-MM-DD HH:MM:SS.mmm)
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H:%M:%S")
        << '.' << std::setw(3) << std::setfill('0') << now_ms.count();

    // Get thread ID
    std::ostringstream thread_id_ss;
    thread_id_ss << std::this_thread::get_id();

    // Output formatted log
    std::cout << oss.str()
              << " [" << level << "]"
              << " [thread " << thread_id_ss.str() << "] "
              << message << std::endl;

}

static void log_structured(const std::string &level, const std::string &message) {
    // Get thread ID
    std::ostringstream thread_id_ss;
    thread_id_ss << std::this_thread::get_id();

    const nlohmann::json log_entry = {
        {"level", level},
        {"message", message},
        {"timestamp", std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now())},
        {"thread", thread_id_ss.str()}
    };

    std::cout << log_entry.dump() << std::endl;
}

static void log(const std::string &level, const std::string &message, bool structured) {
    if (structured) {
        log_structured(level, message);
    } else {
        log_plain(level, message);
    }
}

void log_util::info(const std::string &message) const {
    log("INFO", message, useStructuredLogs);
}

void log_util::warn(const std::string &message) const {
    log("WARN", message, useStructuredLogs);
}

void log_util::error(const std::string &message) const {
    log("ERROR", message, useStructuredLogs);
}

