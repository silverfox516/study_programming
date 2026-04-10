// Ranges factories — C++20
// What: views::iota, views::single, views::empty produce ranges out of thin air,
//       useful as starting points for pipelines or for testing.
// Why:  loops over a numeric range used to need a manual `for (int i = ...)` setup.
// Pre:  hand-written counting loops or boost::counting_iterator.

#include <iostream>
#include <ranges>

int main() {
    // Bounded iota — finite [0, 10)
    for (int n : std::views::iota(0, 10)) std::cout << n << " ";
    std::cout << "\n";

    // Unbounded iota — produces an infinite range, must be capped downstream
    auto first_five_squares = std::views::iota(1)
                            | std::views::transform([](int n) { return n * n; })
                            | std::views::take(5);
    for (int n : first_five_squares) std::cout << n << " ";
    std::cout << "\n";

    // single — a one-element range
    for (int n : std::views::single(42)) std::cout << "single: " << n << "\n";

    // empty<T> — a zero-element range, useful as an identity in chained pipelines
    auto e = std::views::empty<int>;
    std::cout << "empty size: " << std::ranges::distance(e) << "\n";

    return 0;
}
