#include <iostream>
#include <thread>
#include <latch>
#include <barrier>
#include <semaphore>
#include <vector>
#include <chrono>

// std::latch: Single-use countdown
std::latch work_done(3);

void do_work(int id) {
    std::cout << "Worker " << id << " started." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100 * id));
    std::cout << "Worker " << id << " waiting." << std::endl;
    work_done.count_down();
}

// std::counting_semaphore
std::counting_semaphore<2> sem(2); // Max 2 threads at a time

void resource_user(int id) {
    sem.acquire();
    std::cout << "Thread " << id << " entered critical section." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::cout << "Thread " << id << " leaving critical section." << std::endl;
    sem.release();
}

int main() {
    std::cout << "--- Latch Demo ---" << std::endl;
    std::vector<std::thread> workers;
    for (int i = 1; i <= 3; ++i) {
        workers.emplace_back(do_work, i);
    }
    work_done.wait();
    std::cout << "All workers passed latch." << std::endl;
    for (auto& t : workers) t.join();

    std::cout << "\n--- Semaphore Demo ---" << std::endl;
    std::vector<std::thread> users;
    for (int i = 0; i < 5; ++i) {
        users.emplace_back(resource_user, i);
    }
    for (auto& t : users) t.join();

    return 0;
}
