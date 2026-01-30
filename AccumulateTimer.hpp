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
    explicit AccumulateTimer(const std::string& name, TimeUnit unit = TimeUnit::Microseconds, std::ostream& output = std::cout) 
        : _name(name), _total_duration(0), _is_running(false), _unit(unit), _output(output) {}

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

    ~AccumulateTimer() {
        if (_is_running) {
            end();
            std::cerr << "[INFO] " << _name << ": Timer was still running at destruction. Auto-ended." << std::endl;
        }
        _output << createLogString(_total_duration);
    }

    AccumulateTimer(const AccumulateTimer&) = delete;
    AccumulateTimer& operator=(const AccumulateTimer&) = delete;
};

}  // namespace toolbox
