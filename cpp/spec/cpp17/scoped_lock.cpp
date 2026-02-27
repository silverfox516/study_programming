#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::mutex m_a;
std::mutex m_b;

void thread_func_1() {
    // std::scoped_lock locks multiple mutexes safely (avoiding deadlock)
    // using a deadlock-avoidance algorithm (similar to std::lock)
    std::scoped_lock lock(m_a, m_b);
    std::cout << "Thread 1 acquired both locks." << std::endl;
}

void thread_func_2() {
    // Order doesn't matter for correctness with scoped_lock, preventing deadlock
    // if another thread tries recursively locking in different order.
    std::scoped_lock lock(m_b, m_a); 
    std::cout << "Thread 2 acquired both locks." << std::endl;
}

int main() {
    std::thread t1(thread_func_1);
    std::thread t2(thread_func_2);

    t1.join();
    t2.join();

    std::cout << "Finished without deadlock." << std::endl;
    return 0;
}
