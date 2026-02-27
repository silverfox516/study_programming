#include <iostream>
#include <thread>
#include <chrono>

void thread_function() {
    std::cout << "Thread function started" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Thread function finished" << std::endl;
}

int main() {
    std::thread t(thread_function);
    std::cout << "Main thread" << std::endl;
    t.join();
    std::cout << "Thread joined" << std::endl;
    return 0;
}
