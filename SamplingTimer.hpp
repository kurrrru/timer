#pragma once
#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace toolbox {

class SamplingTimer {
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
    bool _is_running;
    TimeUnit _unit;
    std::ostream& _output;
    bool _auto_log;
    std::vector<std::chrono::nanoseconds> _samples;

    static double toUnit(std::chrono::nanoseconds d, TimeUnit u) {
        switch (u) {
            case TimeUnit::Nanoseconds:  return static_cast<double>(d.count());
            case TimeUnit::Microseconds: return d.count() / 1000.0;
            case TimeUnit::Milliseconds: return d.count() / 1e6;
            case TimeUnit::Seconds:      return d.count() / 1e9;
        }
        return 0.0;
    }

public:
    explicit SamplingTimer(const std::string& name, TimeUnit unit = TimeUnit::Microseconds,
            std::ostream& output = std::cout)
        : _name(name), _is_running(false), _unit(unit), _output(output), _auto_log(true) {}

    void reserve(std::size_t n) { _samples.reserve(n); }
    void setAutoLog(bool on)    { _auto_log = on; }

    void start() {
        if (_is_running) { std::cerr << "[WARNING] " << _name << ": start() while running!\n"; return; }
        _is_running = true;
        _start_tp = Clock::now();
    }
    void end() {
        auto now = Clock::now();
        if (!_is_running) { std::cerr << "[WARNING] " << _name << ": end() without start()!\n"; return; }
        _samples.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(now - _start_tp));
        _is_running = false;
    }
    void reset() { _samples.clear(); _is_running = false; }

    // --- 取得系（出力しない）---
    std::size_t count() const { return _samples.size(); }
    const std::vector<std::chrono::nanoseconds>& samples() const { return _samples; }

    std::chrono::nanoseconds total() const {
        std::chrono::nanoseconds s{0};
        for (auto v : _samples) s += v;
        return s;
    }
    double total(TimeUnit u) const {
        return toUnit(total(), u);
    }
    double mean(TimeUnit u) const {
        if (_samples.empty()) return 0.0;
        return toUnit(total(), u) / static_cast<double>(_samples.size());
    }
    double percentile(double p, TimeUnit u) const {  // p in [0,100], 線形補間
        if (_samples.empty()) return 0.0;
        if (p < 0.0 || p > 100.0) {
            std::cerr << "[WARNING] " << _name << ": percentile(" << p << ") must be in [0,100]!\n";
            return 0.0;
        }
        std::vector<std::chrono::nanoseconds> s(_samples);
        std::sort(s.begin(), s.end());
        double rank = (p / 100.0) * (s.size() - 1);
        std::size_t lo = static_cast<std::size_t>(rank);
        double frac = rank - lo;
        double a = toUnit(s[lo], u);
        double b = toUnit(s[std::min(lo + 1, s.size() - 1)], u);
        return a + (b - a) * frac;
    }
    double median(TimeUnit u) const {
        return percentile(50.0, u);
    }
    double stddev(TimeUnit u) const {  // 不偏標準偏差(unbiased standard deviation)(n-1)
        if (_samples.size() < 2) {
            return 0.0;
        }
        double m = mean(u), acc = 0.0;
        for (auto v : _samples) {
            double x = toUnit(v, u) - m;
            acc += x * x;
        }
        return std::sqrt(acc / (_samples.size() - 1));
    }

    ~SamplingTimer() {
        if (_is_running) {
            end();
            std::cerr << "[INFO] " << _name << ": Timer was still running at destruction. Auto-ended." << std::endl;
        }
        if (!_auto_log || _samples.empty()) return;
        std::ostringstream oss;
        oss << "[STATS] " << _name
            << " n=" << count()
            << " mean=" << mean(_unit)
            << " p50=" << median(_unit)
            << " p95=" << percentile(95.0, _unit)
            << " p99=" << percentile(99.0, _unit) << "\n";
        _output << oss.str();
    }

    SamplingTimer(const SamplingTimer&)            = delete;
    SamplingTimer& operator=(const SamplingTimer&) = delete;
    SamplingTimer(SamplingTimer&&)                 = delete;
    SamplingTimer& operator=(SamplingTimer&&)      = delete;
};

}  // namespace toolbox
