// std::views::repeat — C++23
// What: a range that yields the same value over and over, optionally bounded
//       by a count.
// Why:  generates a constant sequence for piping into other range operations,
//       and replaces the C++17 std::fill / std::generate boilerplate.
// Pre:  std::vector<T>(n, value) or std::fill_n loops.
//
// libc++ feature test: __cpp_lib_ranges_repeat (Apple clang 21: NOT defined)

#include <iostream>
#include <version>

#if defined(__cpp_lib_ranges_repeat) && __cpp_lib_ranges_repeat >= 202207L
#  include <ranges>
#  define HAVE_REP 1
#else
#  define HAVE_REP 0
#endif

int main() {
#if HAVE_REP
    // Bounded: 5 copies of 42
    for (int n : std::views::repeat(42, 5)) std::cout << n << " ";
    std::cout << "\n";

    // Unbounded — must be capped downstream
    for (int n : std::views::repeat('A') | std::views::take(3)) std::cout << char(n);
    std::cout << "\n";
#else
    std::cout << "<ranges> std::views::repeat not available in this libc++.\n";
    for (int i = 0; i < 5; ++i) std::cout << 42 << " ";
    std::cout << "\n";
#endif
    return 0;
}
