// std::gcd / std::lcm — C++17
// What: greatest common divisor and least common multiple in <numeric>.
// Why:  every project used to ship its own. Now they are standard, constexpr, and correct.
// Pre:  hand-written Euclid's algorithm with subtle integer-overflow bugs.

#include <iostream>
#include <numeric>

int main() {
    std::cout << "gcd(12, 18) = " << std::gcd(12, 18) << "\n";
    std::cout << "gcd(17, 13) = " << std::gcd(17, 13) << " (coprime)\n";
    std::cout << "lcm(4,  6)  = " << std::lcm(4, 6)   << "\n";
    std::cout << "lcm(7, 11)  = " << std::lcm(7, 11)  << "\n";

    // Both are constexpr
    constexpr int g = std::gcd(120, 36);
    constexpr int l = std::lcm(120, 36);
    static_assert(g == 12, "");
    static_assert(l == 360, "");
    std::cout << "constexpr gcd(120,36) = " << g << ", lcm = " << l << "\n";

    return 0;
}
