// constexpr unique_ptr — C++23
// What: std::unique_ptr (and many other utilities) are now usable in constexpr
//       contexts. Allocations made at compile time must also be freed at compile time.
// Why:  enables real, allocating algorithms (vectors, hash maps, ...) inside
//       constexpr functions, opening the door to richer compile-time computation.
// Pre:  constexpr was limited to non-allocating, deeply restricted code.

#include <iostream>
#include <memory>

constexpr int compute() {
    auto p = std::make_unique<int>(42);
    int  v = *p;
    *p = v * v;
    return *p;
    // p is destroyed here — the compile-time allocator must also free here
}

int main() {
    constexpr int answer = compute();
    static_assert(answer == 1764, "");
    std::cout << "constexpr compute() = " << answer << "\n";
    return 0;
}
