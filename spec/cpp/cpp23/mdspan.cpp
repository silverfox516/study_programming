// std::mdspan — C++23
// What: a multidimensional non-owning view over a contiguous array, with
//       compile-time or runtime extents and pluggable layout policies.
// Why:  numeric / scientific code can finally talk about "this raw buffer is a
//       4x3 matrix" without inventing a custom wrapper or pulling in a library.
// Pre:  hand-rolled Matrix wrappers, or external libraries like Eigen / xtensor.

#include <iostream>
#include <mdspan>
#include <vector>

int main() {
    // Underlying linear storage
    std::vector<int> buffer{
        1, 2, 3,
        4, 5, 6,
        7, 8, 9,
       10,11,12,
    };

    // 4 rows × 3 cols, dynamic extents
    std::mdspan<int, std::dextents<std::size_t, 2>> m(buffer.data(), 4, 3);

    std::cout << "extent(0) = " << m.extent(0) << "\n";
    std::cout << "extent(1) = " << m.extent(1) << "\n";

    // C++23 multi-arg subscript notation
    for (std::size_t i = 0; i < m.extent(0); ++i) {
        for (std::size_t j = 0; j < m.extent(1); ++j) {
            std::cout << m[i, j] << ' ';
        }
        std::cout << "\n";
    }

    // Compile-time extents — fully static, no per-instance dimensions
    std::mdspan<int, std::extents<std::size_t, 2, 6>> m2(buffer.data());
    std::cout << "static view: " << m2.extent(0) << "x" << m2.extent(1) << "\n";

    return 0;
}
