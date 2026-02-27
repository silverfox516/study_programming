#include <iostream>

struct MyClass {
    static inline int value = 10;
};

int main() {
    std::cout << MyClass::value << std::endl;
    MyClass::value = 20;
    std::cout << MyClass::value << std::endl;
    return 0;
}
