// std::ranges::to — C++23
// What: pipe a range into a concrete container type, materializing the lazy view.
// Why:  C++20 ranges produced views that were inconvenient to convert back to
//       std::vector. ranges::to closes that gap and lets pipelines end naturally.
// Pre:  manual loops or `std::vector v(view.begin(), view.end())` boilerplate.

#include <iostream>
#include <ranges>
#include <vector>
#include <set>
#include <string>

int main() {
    // Pipe a transformed iota directly into a vector
    auto squares = std::views::iota(1, 6)
                 | std::views::transform([](int n) { return n * n; })
                 | std::ranges::to<std::vector>();

    for (int x : squares) std::cout << x << " ";
    std::cout << "\n";

    // Works with any container — set, deque, list...
    auto unique_lengths = std::vector<std::string>{"hi", "hey", "hi", "hello", "hey"}
                        | std::views::transform([](const std::string& s) { return s.size(); })
                        | std::ranges::to<std::set>();

    std::cout << "unique lengths: ";
    for (auto n : unique_lengths) std::cout << n << " ";
    std::cout << "\n";

    return 0;
}
