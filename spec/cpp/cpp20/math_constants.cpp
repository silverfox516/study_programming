#include <iostream>
#include <numbers>
#include <cmath>

int main() {
    std::cout << "Pi: " << std::numbers::pi << std::endl;
    std::cout << "e:  " << std::numbers::e << std::endl;
    
    double radius = 5.0;
    double area = std::numbers::pi * std::pow(radius, 2);
    
    std::cout << "Area of circle with radius 5: " << area << std::endl;

    return 0;
}
