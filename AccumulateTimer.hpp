#pragma once
#include <iostream>
#include <chrono>
#include <string>
#include <iomanip>
#include <sstream>

namespace toolbox {

class AccumulateTimer {
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
    std::chrono::nanoseconds _total_duration;
    bool _is_running;
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
    explicit AccumulateTimer(const std::string& name, TimeUnit unit = TimeUnit::Microseconds, std::ostream& output = std::cout) 
        : _name(name), _total_duration(0), _is_running(false), _unit(unit), _output(output), _auto_log(true) {}

    void start() {
        if (_is_running) {
            std::cerr << "[WARNING] " << _name << ": start() called while already running!" << std::endl;
            return;
        }
        _is_running = true;
        _start_tp = Clock::now();
    }

    void end() {
        auto end_tp = Clock::now();
        if (!_is_running) {
            std::cerr << "[WARNING] " << _name << ": end() called without start()!" << std::endl;
            return;
        }
        _total_duration += std::chrono::duration_cast<std::chrono::nanoseconds>(end_tp - _start_tp);
        _is_running = false;
    }

    std::chrono::nanoseconds elapsed() const {
        auto total = _total_duration;
        if (_is_running) {
            total += std::chrono::duration_cast<std::chrono::nanoseconds>(
                Clock::now() - _start_tp);
        }
        return total;
    }

    double elapsed(TimeUnit unit) const {
        return toUnit(elapsed(), unit);
    }

    void setAutoLog(bool on) { _auto_log = on; }

    ~AccumulateTimer() {
        if (_is_running) {
            end();
            std::cerr << "[INFO] " << _name << ": Timer was still running at destruction. Auto-ended." << std::endl;
        }
        if (_auto_log) {
            _output << createLogString(_total_duration);
        }
    }

    AccumulateTimer(const AccumulateTimer&) = delete;
    AccumulateTimer& operator=(const AccumulateTimer&) = delete;
};

}  // namespace toolbox
