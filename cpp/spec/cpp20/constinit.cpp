#include <iostream>

constinit int value = 42;

int main() {
    std::cout << "Value: " << value << std::endl;
    // value = 10; // Error: value is constinit, so it's const
    return 0;
}
