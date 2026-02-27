#include <iostream>
#include <format>

int main() {
    std::string s = std::format("Hello, {}!", "World");
    std::cout << s << std::endl;

    int value = 42;
    std::cout << std::format("The answer is {}.", value) << std::endl;
    
    return 0;
}
