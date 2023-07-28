#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <iomanip> // 包含输入/输出流格式化库
#include <cstdio> // 包含格式化字符串函数

// 定义日志级别
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

// 定义日志宏
#define LOG(level, format, ...) \
    do { \
        if (level >= LOG_LEVEL_THRESHOLD) { \
            char buffer[256]; \
            std::sprintf(buffer, format, ##__VA_ARGS__); \
            std::cout << "[" << logLevelToString(level) << "] " \
                      << __FILE__ << ":" << __LINE__ << " - " \
                      << getCurrentTime() << " - " \
                      << buffer << std::endl; \
        } \
    } while (false)

// 定义日志级别阈值
constexpr LogLevel LOG_LEVEL_THRESHOLD = LogLevel::DEBUG;

// 将日志级别转换为字符串    内联展开插入到调用的地方
inline std::string logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

// 获取当前时间的字符串表示
inline std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

//int main() {
//    int num = 42;
//    std::string name = "John Doe";
//
//    LOG(LogLevel::INFO, "Number: %d, Name: %s", num, name.c_str());
//    LOG(LogLevel::WARNING, "This is a warning message");
//    LOG(LogLevel::ERROR, "This is an error message");
//
//    return 0;
//}

#endif