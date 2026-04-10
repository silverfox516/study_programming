// std::barrier — C++20
// What: a reusable thread synchronization point. Unlike latch, it can be cycled
//       through multiple "phases", and an optional completion function runs
//       once per phase when all participants arrive.
// Why:  the natural way to express "phase 1, then everyone catches up, then phase 2".
// Pre:  hand-rolled with mutex + condition variable + counter, hard to get right.
// See also: spec/cpp/cpp20/sync_primitives.cpp covers std::latch and counting_semaphore.

#include <iostream>
#include <barrier>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>

std::mutex io_mtx;

int main() {
    constexpr int num_workers = 4;
    int phase = 0;

    auto on_completion = [&]() noexcept {
        std::lock_guard lock(io_mtx);
        std::cout << "--- end of phase " << ++phase << " ---\n";
    };

    std::barrier sync(num_workers, on_completion);

    auto worker = [&](int id) {
        for (int p = 1; p <= 3; ++p) {
            {
                std::lock_guard lock(io_mtx);
                std::cout << "worker " << id << " doing phase " << p << "\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10 * id));
            sync.arrive_and_wait();   // wait until all four arrive
        }
    };

    std::vector<std::thread> ts;
    for (int i = 0; i < num_workers; ++i) ts.emplace_back(worker, i);
    for (auto& t : ts) t.join();
    return 0;
}
