// C++17: std::not_fn — negates any callable (replaces deprecated not1/not2)

#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>
#include <string>

bool is_even(int n) { return n % 2 == 0; }

struct IsPositive {
    bool operator()(int n) const { return n > 0; }
};

int main() {
    std::vector<int> v{-3, -1, 0, 2, 4, 5, 7, 8};

    // not_fn with function pointer
    auto is_odd = std::not_fn(is_even);
    auto odd_count = std::count_if(v.begin(), v.end(), is_odd);
    std::cout << "Odd count: " << odd_count << "\n";

    // not_fn with functor
    auto is_non_positive = std::not_fn(IsPositive{});
    auto non_pos = std::count_if(v.begin(), v.end(), is_non_positive);
    std::cout << "Non-positive count: " << non_pos << "\n";

    // not_fn with lambda
    auto is_short = [](const std::string& s) { return s.size() < 4; };
    auto is_long = std::not_fn(is_short);

    std::vector<std::string> words{"hi", "hello", "ok", "world", "cpp"};
    auto long_count = std::count_if(words.begin(), words.end(), is_long);
    std::cout << "Long words (>=4 chars): " << long_count << "\n";

    // partition using not_fn
    std::partition(v.begin(), v.end(), std::not_fn(is_even));
    std::cout << "After partition (odd first): ";
    for (int x : v) std::cout << x << " ";
    std::cout << "\n";

    return 0;
}
