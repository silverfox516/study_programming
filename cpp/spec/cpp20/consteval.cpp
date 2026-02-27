#include <iostream>

consteval int sq(int n) {
    return n * n;
}

int main() {
    constexpr int squared = sq(5);
    std::cout << "Squared: " << squared << std::endl;
    
    // int x = 10;
    // int y = sq(x); // Error: sq must be called with a constant expression

    return 0;
}
