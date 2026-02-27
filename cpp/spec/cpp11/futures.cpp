#include <iostream>
#include <future>
#include <thread>
#include <chrono>

int calculate_meaning_of_life() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 42;
}

void promise_setter(std::promise<int> p) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    p.set_value(100);
}

int main() {
    // std::async
    std::cout << "Starting async task..." << std::endl;
    std::future<int> f1 = std::async(std::launch::async, calculate_meaning_of_life);
    
    std::cout << "Doing other work while waiting..." << std::endl;
    
    int result1 = f1.get(); // Blocks until ready
    std::cout << "Async result: " << result1 << std::endl;

    // std::promise and std::future
    std::promise<int> p;
    std::future<int> f2 = p.get_future();
    
    std::thread t(promise_setter, std::move(p));
    
    std::cout << "Waiting for promise..." << std::endl;
    int result2 = f2.get();
    std::cout << "Promise result: " << result2 << std::endl;
    
    t.join();

    return 0;
}
