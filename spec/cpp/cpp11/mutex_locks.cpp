#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

std::mutex mtx;
int shared_counter = 0;

void increment(int id) {
    for (int i = 0; i < 100; ++i) {
        // std::lock_guard is a RAII wrapper for owning a mutex scope
        std::lock_guard<std::mutex> lock(mtx);
        ++shared_counter;
        // mtx is automatically released when lock goes out of scope
    }
}

void increment_with_unique_lock(int id) {
    for (int i = 0; i < 100; ++i) {
        // std::unique_lock is more flexible than lock_guard
        std::unique_lock<std::mutex> lock(mtx);
        ++shared_counter;
        lock.unlock(); // Can explicitly unlock
        // Do some work without the lock...
        lock.lock(); // And lock again
    }
}

int main() {
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(increment, i);
    }
    
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(increment_with_unique_lock, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Final counter value: " << shared_counter << std::endl;
    return 0;
}
