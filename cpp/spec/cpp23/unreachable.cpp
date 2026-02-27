#include <iostream>
#include <utility>

// This function should only be called with values 0, 1, or 2
void process_value(int val) {
    switch (val) {
        case 0:
            std::cout << "Value is 0" << std::endl;
            break;
        case 1:
            std::cout << "Value is 1" << std::endl;
            break;
        case 2:
            std::cout << "Value is 2" << std::endl;
            break;
        default:
            // The compiler knows this point is unreachable,
            // which can help with optimization.
            std::unreachable();
    }
}

int main() {
    process_value(1);
    // process_value(5); // This would be undefined behavior
    return 0;
}
