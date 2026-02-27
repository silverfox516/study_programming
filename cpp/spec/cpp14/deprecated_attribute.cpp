#include <iostream>

[[deprecated("Use new_function() instead")]]
void old_function() {
    std::cout << "This is an old function." << std::endl;
}

void new_function() {
    std::cout << "This is the new function." << std::endl;
}

int main() {
    old_function();
    new_function();
    return 0;
}
