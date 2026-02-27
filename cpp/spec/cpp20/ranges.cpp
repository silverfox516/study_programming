#include <iostream>
#include <vector>
#include <ranges>

int main() {
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto even_numbers = numbers | std::views::filter([](int n) { return n % 2 == 0; });
    auto squared_evens = even_numbers | std::views::transform([](int n) { return n * n; });

    for (int n : squared_evens) {
        std::cout << n << " ";
    }
    std::cout << std::endl;

    return 0;
}
