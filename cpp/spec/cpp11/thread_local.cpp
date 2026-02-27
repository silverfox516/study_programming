#include <iostream>
#include <thread>
#include <vector>

thread_local int g_val = 0;

void thread_func(int id) {
    g_val = id;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Thread " << id << ", g_val = " << g_val << std::endl;
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 1; i <= 5; ++i) {
        threads.emplace_back(thread_func, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Main thread, g_val = " << g_val << std::endl;

    return 0;
}
