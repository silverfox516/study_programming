#include <iostream>
#include <stdexcept>

void may_throw() {
    throw std::runtime_error("error");
}

void no_throw() noexcept {
    // throw std::runtime_error("error"); // This would cause std::terminate
}

int main() {
    try {
        may_throw();
    } catch (const std::exception& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }

    std::cout << "noexcept(no_throw()): " << noexcept(no_throw()) << std::endl;
    std::cout << "noexcept(may_throw()): " << noexcept(may_throw()) << std::endl;

    no_throw(); // This will not throw

    return 0;
}
