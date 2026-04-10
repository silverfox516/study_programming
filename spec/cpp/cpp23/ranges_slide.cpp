// std::views::slide — C++23
// What: a sliding window of fixed size N, advancing one element at a time.
//       Unlike chunk, the windows overlap.
// Why:  the standard answer to moving averages, n-grams, and rolling stats.
// Pre:  hand-written index loops with two cursors.
//
// libc++ feature test: __cpp_lib_ranges_slide (gated by chunk in libc++)

#include <iostream>
#include <vector>
#include <version>

#if defined(__cpp_lib_ranges_slide) && __cpp_lib_ranges_slide >= 202202L
#  include <ranges>
#  define HAVE_SLIDE 1
#else
#  define HAVE_SLIDE 0
#endif

int main() {
    std::vector<int> v{1, 2, 3, 4, 5, 6};

#if HAVE_SLIDE
    for (auto window : v | std::views::slide(3)) {
        std::cout << "window:";
        for (int n : window) std::cout << ' ' << n;
        std::cout << "\n";
    }
#else
    std::cout << "<ranges> std::views::slide not available in this libc++.\n";
    constexpr std::size_t w = 3;
    for (std::size_t i = 0; i + w <= v.size(); ++i) {
        std::cout << "window:";
        for (std::size_t j = i; j < i + w; ++j) std::cout << ' ' << v[j];
        std::cout << "\n";
    }
#endif
    return 0;
}
