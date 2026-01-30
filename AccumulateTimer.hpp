#include <iostream>
#include <chrono>
#include <string>
#include <iomanip>

class AccumulateTimer {
    using Clock = std::chrono::high_resolution_clock;
    
    std::string _name;
    Clock::time_point _start_tp;
    std::chrono::nanoseconds _total_duration;
    bool _is_running;

public:
    explicit AccumulateTimer(const std::string& name) 
        : _name(name), _total_duration(0), _is_running(false) {}

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
        std::ios_base::fmtflags f(std::cout.flags());
        std::cout << "[TIME] " << std::left << std::setw(30) << _name << ": "
                << std::fixed << std::setprecision(4)
                << _total_duration.count() / 1000.0 << " us" << std::endl;
        std::cout.flags(f);
    }

    AccumulateTimer(const AccumulateTimer&) = delete;
    AccumulateTimer& operator=(const AccumulateTimer&) = delete;
};
