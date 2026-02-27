#include <iostream>
#include <vector>
#include <algorithm>
#include <execution>

int main() {
    std::vector<int> v(1000000);
    std::fill(v.begin(), v.end(), 1);

    // Parallel sort
    std::sort(std::execution::par, v.begin(), v.end());

    std::cout << "Vector sorted in parallel." << std::endl;

    return 0;
}
