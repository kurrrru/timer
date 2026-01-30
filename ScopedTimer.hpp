#pragma once
#include <iostream>
#include <chrono>
#include <string>
#include <iomanip>
#include <sstream>

namespace toolbox {

class ScopedTimer {
public:
    using Clock = std::chrono::steady_clock;

    enum class TimeUnit {
        Nanoseconds,
        Microseconds,
        Milliseconds,
        Seconds
    };

private:
    std::string _name;
    Clock::time_point _start_tp;
    TimeUnit _unit;
    std::ostream& _output;

    std::string createLogString(std::chrono::nanoseconds duration) const {
        std::ostringstream oss;
        oss << "[TIME] " << std::left << std::setw(30) << _name << ": "
            << std::fixed << std::setprecision(4);

        switch (_unit) {
            case TimeUnit::Nanoseconds:
                oss << duration.count() << " ns";
                break;
            case TimeUnit::Microseconds:
                oss << duration.count() / 1000.0 << " us";
                break;
            case TimeUnit::Milliseconds:
                oss << duration.count() / 1000000.0 << " ms";
                break;
            case TimeUnit::Seconds:
                oss << duration.count() / 1000000000.0 << " s";
                break;
        }
        oss << "\n";
        return oss.str();
    }

public:
    explicit ScopedTimer(const std::string& name, TimeUnit unit = TimeUnit::Microseconds, std::ostream& output = std::cout)
        : _name(name), _start_tp(Clock::now()), _unit(unit), _output(output) {}

    ~ScopedTimer() {
        auto end_tp = Clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_tp - _start_tp);
        _output << createLogString(duration);
    }

    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;
};

}  // namespace toolbox
