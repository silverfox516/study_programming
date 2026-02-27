#include <iostream>

void process_value(int val) {
    switch (val) {
        case 1:
            std::cout << "Case 1" << std::endl;
            [[fallthrough]]; // Intentionally fall through to case 2
        case 2:
            std::cout << "Case 2" << std::endl;
            break;
        case 3:
            std::cout << "Case 3" << std::endl;
            break;
    }
}

int main() {
    [[maybe_unused]] int unused_var = 10; // No warning for unused variable

    process_value(1);

    return 0;
}
