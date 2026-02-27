#include <iostream>

[[nodiscard]]
int get_value() {
    return 42;
}

int main() {
    get_value(); // Warning: ignoring return value of function declared with 'nodiscard' attribute
    
    int value = get_value();
    std::cout << "Value: " << value << std::endl;

    return 0;
}
