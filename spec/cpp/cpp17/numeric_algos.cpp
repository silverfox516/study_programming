#include <iostream>
#include <numeric>
#include <algorithm>

int main() {
    // std::clamp
    int v = 10;
    std::cout << "clamped(10, 0, 5): " << std::clamp(v, 0, 5) << std::endl;
    std::cout << "clamped(10, 0, 20): " << std::clamp(v, 0, 20) << std::endl;

    // std::gcd and std::lcm
    int a = 12;
    int b = 18;
    std::cout << "gcd(12, 18): " << std::gcd(a, b) << std::endl; // 6
    std::cout << "lcm(12, 18): " << std::lcm(a, b) << std::endl; // 36

    return 0;
}
