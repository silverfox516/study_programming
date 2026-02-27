#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

std::atomic<int> atomic_counter(0);
int non_atomic_counter = 0;

void increment() {
    for (int i = 0; i < 1000; ++i) {
        atomic_counter++;
        non_atomic_counter++; // Data race!
    }
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(increment);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Atomic counter: " << atomic_counter << " (Expected: 10000)" << std::endl;
    std::cout << "Non-atomic counter: " << non_atomic_counter << " (Likely incorrect)" << std::endl;

    return 0;
}
