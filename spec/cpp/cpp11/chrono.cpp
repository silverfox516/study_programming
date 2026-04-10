// <chrono> — C++11
// What: type-safe durations, time points, and clocks for measuring and representing time.
// Why:  replaces ad-hoc int-seconds / time_t / clock_t APIs that lost units and overflowed silently.
// Pre:  C-style time.h with no compile-time unit checking.

#include <iostream>
#include <chrono>
#include <thread>

int main() {
    using namespace std::chrono;

    // Durations have explicit units encoded in the type
    seconds      one_sec(1);
    milliseconds half_sec(500);
    auto         total = one_sec + half_sec;  // implicit conversion to common unit
    std::cout << "1s + 500ms = " << total.count() << " ms\n";

    // Convert between units explicitly with duration_cast
    auto in_us = duration_cast<microseconds>(total);
    std::cout << "in microseconds: " << in_us.count() << "\n";

    // Measure elapsed wall-clock time with steady_clock (monotonic, never goes backwards)
    auto start = steady_clock::now();
    std::this_thread::sleep_for(milliseconds(20));
    auto end = steady_clock::now();
    auto elapsed = duration_cast<milliseconds>(end - start);
    std::cout << "slept for ~" << elapsed.count() << " ms\n";

    // system_clock can convert to/from time_t for I/O with C APIs
    auto now    = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);
    std::cout << "current time_t: " << t << "\n";

    return 0;
}
