#include <iostream>
#include <vector>
#include <algorithm>

// Function to print an integer
void printNumber(int n) {
    std::cout << n << " ";
}

int main() {
    std::vector<int> numbers;
    numbers.push_back(1);
    numbers.push_back(2);
    numbers.push_back(3);
    numbers.push_back(4);
    numbers.push_back(5);

    std::cout << "Numbers: ";
    std::for_each(numbers.begin(), numbers.end(), printNumber);
    std::cout << std::endl;

    return 0;
}
