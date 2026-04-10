// std::atomic_ref — C++20
// What: a wrapper that gives atomic operations on an existing non-atomic object.
//       Lets you opt into atomicity at the access site, not at the declaration site.
// Why:  some objects must be plain (e.g. members of trivially-copyable structs sent
//       over the network) but still need occasional atomic access from threads.
// Pre:  std::atomic<T> changed the type of the underlying storage, which was viral.

#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

int main() {
    int counter = 0;   // plain int, not std::atomic<int>

    std::vector<std::thread> ts;
    for (int i = 0; i < 8; ++i) {
        ts.emplace_back([&counter]{
            // Construct an atomic_ref over the same object
            std::atomic_ref<int> a(counter);
            for (int j = 0; j < 10000; ++j) {
                a.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }
    for (auto& t : ts) t.join();

    std::cout << "final counter = " << counter << " (expected 80000)\n";

    // The underlying object remains a plain int — no type change
    static_assert(std::is_same_v<decltype(counter), int>, "");

    return 0;
}
