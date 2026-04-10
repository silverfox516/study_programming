// std::views::adjacent / adjacent_transform — C++23
// What: a sliding window of fixed size N over a range, yielding tuples of N
//       consecutive elements.
// Why:  pair-wise / triple-wise differences, smoothing windows, and similar
//       neighbour-touching algorithms become one-liners.
// Pre:  manual index loops touching i and i+1, easy to off-by-one.
//
// libc++ feature test: __cpp_lib_ranges_zip (gates the adjacent family too)

#include <iostream>
#include <vector>
#include <version>

#if defined(__cpp_lib_ranges_zip) && __cpp_lib_ranges_zip >= 202110L
#  include <ranges>
#  define HAVE_ADJ 1
#else
#  define HAVE_ADJ 0
#endif

int main() {
    std::vector<int> v{1, 3, 6, 10, 15, 21};

#if HAVE_ADJ
    // Pair-wise differences via adjacent<2>
    for (auto [a, b] : v | std::views::adjacent<2>) {
        std::cout << "(" << a << "," << b << ") diff=" << (b - a) << "\n";
    }

    // adjacent_transform<2>(op) directly applies the operation
    for (int d : v | std::views::adjacent_transform<2>([](int a, int b) { return b - a; })) {
        std::cout << "diff " << d << "\n";
    }
#else
    std::cout << "<ranges> std::views::adjacent not available in this libc++.\n";
    for (std::size_t i = 1; i < v.size(); ++i) {
        std::cout << "(" << v[i - 1] << "," << v[i] << ") diff=" << v[i] - v[i - 1] << "\n";
    }
#endif
    return 0;
}
