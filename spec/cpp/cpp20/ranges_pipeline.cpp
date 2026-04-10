// Ranges pipeline — C++20
// What: chain views with `|` to express data transformations declaratively.
// Why:  replaces nested loops with a readable left-to-right pipeline that mirrors
//       how you'd describe the transformation in English.
// Pre:  imperative loops with index variables and intermediate vectors.
// See also: spec/cpp/cpp20/ranges.cpp covers the basic filter+transform pipeline.

#include <iostream>
#include <ranges>
#include <vector>
#include <string>

int main() {
    std::vector<int> nums{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Drop the first 2, take the next 4, double them, then reverse
    auto pipeline = nums
                  | std::views::drop(2)
                  | std::views::take(4)
                  | std::views::transform([](int n) { return n * 2; })
                  | std::views::reverse;

    std::cout << "pipeline: ";
    for (int n : pipeline) std::cout << n << " ";
    std::cout << "\n";

    // Pipelines compose with non-numeric data
    std::vector<std::string> words{"apple", "be", "cat", "dolphin", "ee"};
    auto long_uppercase_first = words
                              | std::views::filter([](const std::string& s) { return s.size() > 2; })
                              | std::views::transform([](const std::string& s) { return s.front(); });

    std::cout << "first letter of long words: ";
    for (char c : long_uppercase_first) std::cout << c << " ";
    std::cout << "\n";

    // Lazy evaluation — nothing is materialized until iteration
    auto lazy = std::views::iota(1)
              | std::views::filter([](int n) { return n % 7 == 0; })
              | std::views::take(3);
    std::cout << "first 3 multiples of 7: ";
    for (int n : lazy) std::cout << n << " ";
    std::cout << "\n";

    return 0;
}
