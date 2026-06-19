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
    bool _auto_log;

    static double toUnit(std::chrono::nanoseconds d, TimeUnit unit) {
        switch (unit) {
            case TimeUnit::Nanoseconds:  return static_cast<double>(d.count());
            case TimeUnit::Microseconds: return d.count() / 1000.0;
            case TimeUnit::Milliseconds: return d.count() / 1000000.0;
            case TimeUnit::Seconds:      return d.count() / 1e9;
        }
        return 0.0;
    }

    std::string createLogString(std::chrono::nanoseconds duration) const {
        std::ostringstream oss;
        oss << "[TIME] " << std::left << std::setw(30) << _name << ": "
            << std::fixed << std::setprecision(4);
        oss << toUnit(duration, _unit);
        switch (_unit) {
            case TimeUnit::Nanoseconds:
                oss  << " ns";
                break;
            case TimeUnit::Microseconds:
                oss << " us";
                break;
            case TimeUnit::Milliseconds:
                oss << " ms";
                break;
            case TimeUnit::Seconds:
                oss << " s";
                break;
        }
        oss << "\n";
        return oss.str();
    }

public:
    explicit ScopedTimer(const std::string& name, TimeUnit unit = TimeUnit::Microseconds, std::ostream& output = std::cout)
        : _name(name), _start_tp(Clock::now()), _unit(unit), _output(output), _auto_log(true) {}

    ~ScopedTimer() {
        if (!_auto_log) return;
        auto duration = elapsed();
        _output << createLogString(duration);
    }

    std::chrono::nanoseconds elapsed() const {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            Clock::now() - _start_tp);
    }

    double elapsed(TimeUnit unit) const {
        return toUnit(elapsed(), unit);
    }

    void setAutoLog(bool on) { _auto_log = on; }

    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;
};

}  // namespace toolbox
