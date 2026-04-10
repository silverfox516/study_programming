// 1uz / 1z literal suffix — C++23
// What: integer literal suffixes that produce `std::size_t` (uz) or its signed
//       counterpart (z).
// Why:  comparing int loop indices against `.size()` triggered sign-conversion
//       warnings forever. The new suffix lets you write the right type directly.
// Pre:  static_cast<std::size_t>(0) or `0u` followed by an explicit cast.

#include <iostream>
#include <vector>
#include <type_traits>

int main() {
    auto a = 5uz;
    auto b = 5z;

    static_assert(std::is_same_v<decltype(a), std::size_t>, "");
    static_assert(std::is_same_v<decltype(b), std::ptrdiff_t>, "");

    std::vector<int> v(10);
    for (auto i = 0uz; i < v.size(); ++i) {
        v[i] = static_cast<int>(i * i);
    }

    std::cout << "v[5] = " << v[5uz] << "\n";
    std::cout << "size = " << v.size() << "\n";

    // No more sign-mismatch warnings on the comparison: both sides are size_t
    return 0;
}
