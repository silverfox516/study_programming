// auto(x) decay-copy — C++23
// What: `auto(x)` and `auto{x}` create a prvalue copy of x with decayed type.
//       Useful for "snapshot this value into a fresh object" without spelling the type.
// Why:  generic code wanting "a movable copy" used to write
//       `typename std::decay_t<decltype(x)> tmp(x)` — verbose and error-prone.
// Pre:  hand-written decay_t + decltype incantations.

#include <iostream>
#include <type_traits>
#include <vector>

int main() {
    int x = 5;
    auto y = auto(x);   // y is int, a copy of x
    static_assert(std::is_same_v<decltype(y), int>, "");

    const int& cref = x;
    auto z = auto(cref);  // z is int (reference and const stripped)
    static_assert(std::is_same_v<decltype(z), int>, "");
    std::cout << "y = " << y << ", z = " << z << "\n";

    // Common motivation: pass an rvalue copy into a sink function
    auto consume = [](std::vector<int> v) {
        std::cout << "consumed " << v.size() << " elements\n";
    };
    std::vector<int> source{1, 2, 3, 4};
    consume(auto(source));        // copy of source, treated as rvalue
    std::cout << "source still has " << source.size() << " elements\n";

    return 0;
}
