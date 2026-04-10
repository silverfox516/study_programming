// std::stop_token / stop_source / stop_callback — C++20
// What: cooperative cancellation primitives. A stop_source produces tokens; threads
//       check the token periodically and exit when stop is requested.
// Why:  killing a thread is unsafe; cooperative cancellation is the only sane model.
// Pre:  hand-rolled atomic<bool> "should_stop" flags, plus ad-hoc callback chains.
// See also: spec/cpp/cpp20/jthread.cpp — jthread destructor automatically requests stop.

#include <iostream>
#include <stop_token>
#include <thread>
#include <chrono>
#include <atomic>

int main() {
    std::stop_source src;
    std::stop_token  tok = src.get_token();

    // A callback fires when stop is requested — useful for waking blocked I/O
    std::stop_callback cb(tok, []{
        std::cout << "[callback] stop requested, cleaning up\n";
    });

    std::atomic<int> ticks{0};

    // Spawn a worker that checks the token in its loop
    std::thread worker([tok, &ticks]{
        while (!tok.stop_requested()) {
            ++ticks;
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        std::cout << "worker exiting after " << ticks.load() << " ticks\n";
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    src.request_stop();   // triggers the callback and the worker's exit
    worker.join();

    // Tokens are also queryable for state without blocking
    std::cout << "stop_requested: " << tok.stop_requested() << "\n";
    std::cout << "stop_possible:  " << tok.stop_possible()  << "\n";

    return 0;
}
