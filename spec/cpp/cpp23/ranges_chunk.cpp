// std::views::chunk — C++23
// What: split a range into non-overlapping fixed-size sub-ranges.
// Why:  the standard "batch" operation: iterate items in groups of N for paging,
//       fixed-size network frames, or matrix-row partitioning.
// Pre:  manual index loops with `i / chunk_size` arithmetic.
//
// libc++ feature test: __cpp_lib_ranges_chunk (Apple clang 21: NOT defined)

#include <iostream>
#include <vector>
#include <version>

#if defined(__cpp_lib_ranges_chunk) && __cpp_lib_ranges_chunk >= 202202L
#  include <ranges>
#  define HAVE_CHUNK 1
#else
#  define HAVE_CHUNK 0
#endif

int main() {
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

#if HAVE_CHUNK
    int batch = 0;
    for (auto group : v | std::views::chunk(3)) {
        std::cout << "batch " << ++batch << ":";
        for (int n : group) std::cout << ' ' << n;
        std::cout << "\n";
    }
#else
    std::cout << "<ranges> std::views::chunk not available in this libc++.\n";
    constexpr std::size_t chunk_size = 3;
    for (std::size_t i = 0, batch = 0; i < v.size(); i += chunk_size) {
        std::cout << "batch " << ++batch << ":";
        for (std::size_t j = i; j < std::min(i + chunk_size, v.size()); ++j) {
            std::cout << ' ' << v[j];
        }
        std::cout << "\n";
    }
#endif
    return 0;
}
