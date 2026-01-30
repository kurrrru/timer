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

Since this is a header-only library, simply copy the `ScopedTimer.hpp` and `AccumulateTimer.hpp` files into your project's include directory.

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

### Supported Units

* `toolbox::ScopedTimer::TimeUnit::Nanoseconds`
* `toolbox::ScopedTimer::TimeUnit::Microseconds` (Default)
* `toolbox::ScopedTimer::TimeUnit::Milliseconds`
* `toolbox::ScopedTimer::TimeUnit::Seconds`

## Requirements

* C++11 or later

## License

MIT License