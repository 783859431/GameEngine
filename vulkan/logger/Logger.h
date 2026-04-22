#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <mutex>
#include <cstdarg>   
#include <cstdio>    
#include <memory>    
enum PLevel
{
    PTRACE,
    PDEBUG,
    PINFO,
    PWARN,
    PERROR,
    PFATAL
};

class Logger {
private:
    static Logger* instance;
    static std::mutex mtx;

    std::ofstream fileStream;
    PLevel minLevel = PLevel::PINFO;
    bool consoleOutput = true;
    bool fileOutput = false;

    Logger() = default;


    //static std::string getCurrentTime() {
    //    auto now = std::chrono::system_clock::now();
    //    auto time_t = std::chrono::system_clock::to_time_t(now);
    //    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
    //        now.time_since_epoch()) % 1000;

    //    std::ostringstream ss;
    //    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S")
    //        << '.' << std::setfill('0') << std::setw(3) << ms.count();
    //    return ss.str();
    //}

    static std::string levelToString(PLevel level) {
        switch (level) {
        case PLevel::PTRACE: return "TRACE";
        case PLevel::PDEBUG: return "DEBUG";
        case PLevel::PINFO:  return "INFO ";
        case PLevel::PWARN:  return "WARN ";
        case PLevel::PERROR: return "ERROR";
        case PLevel::PFATAL: return "FATAL";
        default:              return "?????";
        }
    }

public:
    static Logger& getInstance() {
        std::lock_guard<std::mutex> lock(mtx);
        if (instance == nullptr) {
            instance = new Logger();
        }
        return *instance;
    }

    void setLevel(PLevel level) { minLevel = level; }
    void enableConsole(bool enable) { consoleOutput = enable; }

    void enableFile(const std::string& filename = "app.log") {
        fileStream.open(filename, std::ios::app);
        fileOutput = fileStream.is_open();
    }

    template<typename... Args>
    void log(PLevel level, const char* format, Args&&... args)
    {
        if (level < minLevel) return;


        int size = std::snprintf(nullptr, 0, format, std::forward<Args>(args)...) + 1;
        if (size <= 0) return;


        std::unique_ptr<char[]> buf(new char[size]);
        std::snprintf(buf.get(), size, format, std::forward<Args>(args)...);

        std::string message = buf.get();


        std::ostringstream ss;
        ss
            << "[" << levelToString(level) << "] "
            << message << "\n";

        std::string fullLog = ss.str();


        if (consoleOutput) {
            std::cout << fullLog;
            if (level >= PLevel::PERROR) {
                std::cerr << fullLog;
            }
        }


        if (fileOutput && fileStream.is_open()) {
            std::lock_guard<std::mutex> lock(mtx);
            fileStream << fullLog;
            fileStream.flush();
        }
    
    }

    template<typename... Args> void trace(const char* fmt, Args&&... args) { log(PLevel::PTRACE, fmt, std::forward<Args>(args)...); }
    template<typename... Args> void debug(const char* fmt, Args&&... args) { log(PLevel::PDEBUG, fmt, std::forward<Args>(args)...); }
    template<typename... Args> void info(const char* fmt, Args&&... args) { log(PLevel::PINFO, fmt, std::forward<Args>(args)...); }
    template<typename... Args> void warn(const char* fmt, Args&&... args) { log(PLevel::PWARN, fmt, std::forward<Args>(args)...); }
    template<typename... Args> void error(const char* fmt, Args&&... args) { log(PLevel::PERROR, fmt, std::forward<Args>(args)...); }
    template<typename... Args> void fatal(const char* fmt, Args&&... args) { log(PLevel::PFATAL, fmt, std::forward<Args>(args)...); }
};
