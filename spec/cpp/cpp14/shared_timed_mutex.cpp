// std::shared_timed_mutex — C++14
// What: a reader/writer mutex with timed-lock support. Multiple readers OR one writer.
// Why:  read-heavy workloads waste throughput on a plain mutex that serializes everything.
// Pre:  pthread_rwlock_t or hand-rolled reader counters with a regular mutex.
// Note: C++17 added the simpler std::shared_mutex without the timed-lock overhead.

#include <iostream>
#include <shared_mutex>
#include <thread>
#include <vector>
#include <chrono>

std::shared_timed_mutex rw_mutex;
int shared_value = 0;

void reader(int id) {
    std::shared_lock<std::shared_timed_mutex> lock(rw_mutex);
    std::cout << "reader " << id << " sees " << shared_value << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void writer(int id, int new_value) {
    std::unique_lock<std::shared_timed_mutex> lock(rw_mutex);
    std::cout << "writer " << id << " setting to " << new_value << "\n";
    shared_value = new_value;
}

int main() {
    std::vector<std::thread> threads;
    threads.emplace_back(writer, 1, 100);
    for (int i = 0; i < 3; ++i) threads.emplace_back(reader, i);
    threads.emplace_back(writer, 2, 200);
    for (int i = 3; i < 6; ++i) threads.emplace_back(reader, i);

    for (auto& t : threads) t.join();
    std::cout << "final value = " << shared_value << "\n";
    return 0;
}
