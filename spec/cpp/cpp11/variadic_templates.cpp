#include <iostream>

// Base case
void print() {
    std::cout << std::endl;
}

// Recursive step
template <typename T, typename... Args>
void print(T first, Args... args) {
    std::cout << first << " ";
    print(args...);
}

int main() {
    print("Hello", 1, 3.14, 'a');
    return 0;
}
