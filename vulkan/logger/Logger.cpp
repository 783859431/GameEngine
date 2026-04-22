#include "Logger.h"
// 静态成员初始化
Logger* Logger::instance = nullptr;
std::mutex Logger::mtx;
