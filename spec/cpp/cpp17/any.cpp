#include <iostream>
#include <any>
#include <string>

int main() {
    std::any a = 5;
    std::cout << std::any_cast<int>(a) << std::endl;

    a = std::string("hello");
    std::cout << std::any_cast<std::string>(a) << std::endl;

    try {
        std::cout << std::any_cast<double>(a) << std::endl;
    } catch (const std::bad_any_cast& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
