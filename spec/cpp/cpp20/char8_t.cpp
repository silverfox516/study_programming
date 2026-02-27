#include <iostream>
#include <string>

int main() {
    const char8_t* s1 = u8"Hello, UTF-8!";
    std::u8string s2 = u8"你好, 世界!";

    // Note: Printing u8string directly to std::cout might not work as expected
    // on all platforms. This example demonstrates the type.
    std::cout << "Size of char8_t: " << sizeof(char8_t) << std::endl;
    std::cout << "s1 is a pointer to char8_t" << std::endl;

    return 0;
}
