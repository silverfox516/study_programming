#include <iostream>
#include <functional>
#include <algorithm>
#include <vector>

int main() {
    std::vector<int> vec;
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);

    // std::bind2nd binds the 2nd argument of std::greater<int> to 15
    int count = std::count_if(vec.begin(), vec.end(), std::bind2nd(std::greater<int>(), 15));
    std::cout << "Count of elements > 15: " << count << std::endl;

    // std::bind1st binds the 1st argument of std::greater<int> to 25
    // i.e., 25 > x
    count = std::count_if(vec.begin(), vec.end(), std::bind1st(std::greater<int>(), 25));
    std::cout << "Count of elements < 25 (25 > x): " << count << std::endl;

    return 0;
}
