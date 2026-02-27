#include <iostream>
#include <array>
#include <algorithm>

int main() {
    std::array<int, 5> arr = {1, 2, 3, 4, 5};

    std::cout << "Size: " << arr.size() << std::endl;

    // Access elements
    std::cout << "Element at index 2: " << arr[2] << std::endl;
    std::cout << "Element at index 3 (at): " << arr.at(3) << std::endl;

    // Range-based for loop
    std::cout << "Elements: ";
    for (const auto& elem : arr) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    // Using algorithms
    std::cout << "Reversed: ";
    std::reverse(arr.begin(), arr.end());
    for (const auto& elem : arr) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    return 0;
}
