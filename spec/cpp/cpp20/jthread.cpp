#include <iostream>
#include <thread>
#include <chrono>

void thread_function(std::stop_token stoken) {
    while (!stoken.stop_requested()) {
        std::cout << "Working..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "Stop requested." << std::endl;
}

int main() {
    std::jthread t(thread_function);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    // t goes out of scope, destructor requests stop and joins.
    return 0;
}
