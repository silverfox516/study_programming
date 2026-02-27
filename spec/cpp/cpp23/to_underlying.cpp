#include <iostream>
#include <utility>
#include <type_traits>

enum class Color : int {
    Red = 1,
    Green = 2,
    Blue = 3
};

enum class Permissions : unsigned char {
    Read = 0x01,
    Write = 0x02,
    Execute = 0x04
};

int main() {
    Color c = Color::Red;
    
    // Before C++23: static_cast
    std::cout << "Color Red value: " << static_cast<int>(c) << std::endl;
    
    // C++23: std::to_underlying
    std::cout << "Color Green value: " << std::to_underlying(Color::Green) << std::endl;

    Permissions p = Permissions::Read;
    auto val = std::to_underlying(p);
    
    std::cout << "Permission value: " << (int)val << std::endl; // Cast to int for print
    std::cout << "Type is same as underlying? " 
              << std::is_same_v<decltype(val), unsigned char> << std::endl;

    return 0;
}
