#include <iostream>

// Literal for degrees to radians
constexpr long double operator"" _deg(long double deg) {
    return deg * 3.14159265358979323846 / 180;
}

int main() {
    long double angle = 90.0_deg;
    std::cout << "90 degrees in radians is " << angle << std::endl;
    return 0;
}
