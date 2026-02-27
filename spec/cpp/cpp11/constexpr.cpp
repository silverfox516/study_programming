#include <iostream>

constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

int main() {
    constexpr int f5 = factorial(5);
    std::cout << "Factorial of 5 is " << f5 << std::endl;

    int x = 6;
    int fx = factorial(x); // Error: x is not a constant expression
    std::cout << "Factorial of 5 is " << fx << std::endl;
    
    return 0;
}
