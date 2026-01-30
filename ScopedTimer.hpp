#pragma once
#include <iostream>
#include <chrono>
#include <string>
#include <iomanip>

class ScopedTimer {
    using Clock = std::chrono::high_resolution_clock;

    std::string _name;
    Clock::time_point _start_tp;

public:
    explicit ScopedTimer(const std::string& name)
        : _name(name), _start_tp(Clock::now()) {}

    ~ScopedTimer() {
        auto end_tp = Clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_tp - _start_tp);

        std::ios_base::fmtflags f(std::cout.flags());
        std::cout << "[TIME] " << std::left << std::setw(30) << _name << ": "
                  << std::fixed << std::setprecision(4)
                  << duration.count() / 1000.0 << " us" << std::endl;
        std::cout.flags(f);
    }

    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;
};
