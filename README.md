# Simple Timer in C++

A lightweight, header-only C++ timer library for benchmarking and performance profiling.  

## Features

* **Header-only:** No build configuration required. Just drop the files into your project.
* **High Precision:** Uses `std::chrono::steady_clock` to guarantee monotonic time measurements (unaffected by system time changes).
* **Low Overhead:** Optimized strictly to exclude logging and conditional logic from the measured time.
* **Flexible Units:** Supports Nanoseconds, Microseconds, Milliseconds, and Seconds.
* **Output Control:** Logs to `std::cout` by default but supports any `std::ostream` (e.g., `std::cerr`, file streams).
* **Thread-Safety Aware:** Log messages are constructed atomically before output to minimize interleaving in multi-threaded environments.

## Installation

Since this is a header-only library, simply copy the header files you need into your project's include directory.

```bash
cp *.hpp /path/to/your/project/

```

## Usage

### 1. ScopedTimer

Measures the execution time of a specific scope (RAII). The timer starts upon creation and logs the duration upon destruction.

```cpp
#include "ScopedTimer.hpp"
#include <thread>

void heavy_computation() {
    // Measure this scope in milliseconds
    toolbox::ScopedTimer timer("Heavy Computation", toolbox::ScopedTimer::TimeUnit::Milliseconds);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
} 
// Output: [TIME] Heavy Computation             : 500.1234 ms

```

### 2. AccumulateTimer

Accumulates time over multiple intervals. Useful for measuring the total time spent in a specific block that is executed repeatedly (e.g., inside a loop).

```cpp
#include "AccumulateTimer.hpp"

void loop_processing() {
    toolbox::AccumulateTimer timer("Loop Processing", toolbox::AccumulateTimer::TimeUnit::Microseconds);

    for (int i = 0; i < 100; ++i) {
        timer.start();
        // ... process something ...
        timer.end();
        
        // Non-measured processing
    }
} 
// Destructor logs the total accumulated time.
// Output: [TIME] Loop Processing               : 12345.6789 us

```

### 3. SamplingTimer

Collects individual timing samples over multiple intervals and reports summary statistics (min, max, mean, median, percentiles, stddev) at destruction.

```cpp
#include "SamplingTimer.hpp"

void loop_processing() {
    toolbox::SamplingTimer timer("Loop Processing", toolbox::SamplingTimer::TimeUnit::Microseconds);
    timer.reserve(100);  // optional: pre-allocate sample storage

    for (int i = 0; i < 100; ++i) {
        timer.start();
        // ... process something ...
        timer.end();
    }
}
// Destructor logs summary statistics.
// Output: [STATS] Loop Processing n=100 total=123456.00 min=1200.00 max=1500.00 stddev=42.31 mean=1234.56 p50=1230.00 p95=1450.00 p99=1490.00

```

## API Reference

All classes are defined in the `toolbox` namespace.

### `toolbox::ScopedTimer`

* **Constructor:** `ScopedTimer(name, unit = Microseconds, output = std::cout)`
* Starts the timer immediately.


* **Destructor:** Stops the timer and logs the duration.

### `toolbox::AccumulateTimer`

* **Constructor:** `AccumulateTimer(name, unit = Microseconds, output = std::cout)`
* Initializes the timer (does not start it).


* **`start()`**: Resumes timing.
* **`end()`**: Pauses timing and adds the interval to the total.
* **Destructor:** Logs the total accumulated duration. (Auto-ends if still running).

### `toolbox::SamplingTimer`

* **Constructor:** `SamplingTimer(name, unit = Microseconds, output = std::cout)`
* Initializes the timer (does not start it).

* **`start()`**: Starts a new timing interval.
* **`end()`**: Ends the current interval and records the sample.
* **`reserve(n)`**: Pre-allocates storage for `n` samples (optional optimization).
* **`setAutoLog(on)`**: Enables or disables automatic stats output at destruction (default: `true`).
* **`count()`**: Returns the number of recorded samples.
* **`samples()`**: Returns a const reference to the raw sample vector (in `std::chrono::nanoseconds`).
* **`total(unit)`**, **`min(unit)`**, **`max(unit)`**, **`mean(unit)`**: Summary stats in the given unit.
* **`median(unit)`**: Alias for `percentile(50.0, unit)`.
* **`percentile(p, unit)`**: p-th percentile (p in [0, 100]) using linear interpolation.
* **`stddev(unit)`**: Unbiased standard deviation (n−1).
* **Destructor:** Logs `[STATS]` with n, total, min, max, stddev, mean, p50, p95, p99. (Auto-ends if still running; silent if no samples were recorded).

### Supported Units

All three classes share the same set of time units via their respective nested `TimeUnit` enum:

* `Nanoseconds`
* `Microseconds` (Default)
* `Milliseconds`
* `Seconds`

Example: `toolbox::SamplingTimer::TimeUnit::Milliseconds`

## Requirements

* C++11 or later

## License

MIT License