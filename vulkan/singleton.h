#pragma once
template<typename T>
class Singleton {
public:
    static T& Instance() {
        static T instance; // C++11 起线程安全
        return instance;
    }

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

protected:
    Singleton() = default;
    ~Singleton() = default;
};