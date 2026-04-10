// Concepts and requires clauses — C++20
// What: a `concept` names a compile-time predicate on types; a `requires` clause
//       constrains a template to types that satisfy the predicate.
// Why:  replaces SFINAE / enable_if hacks with readable, diagnostic-friendly constraints.
// Pre:  template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>...

#include <iostream>
#include <concepts>
#include <type_traits>
#include <vector>

// Define a concept directly with requires-expression
template <typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
};

// Use a concept as a template parameter constraint
template <Addable T>
T sum(T a, T b) {
    return a + b;
}

// Equivalent forms — pick whichever reads best
template <typename T>
    requires Addable<T>
T sum2(T a, T b) {
    return a + b;
}

// Compound concept built from standard concepts
template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template <Numeric T>
T half(T x) { return x / T(2); }

// requires-expression with multiple requirements
template <typename C>
concept HasSize = requires(C c) {
    { c.size() } -> std::convertible_to<std::size_t>;
    typename C::value_type;
};

template <HasSize C>
void describe(const C& c) {
    std::cout << "size = " << c.size() << "\n";
}

int main() {
    std::cout << "sum(1, 2)        = " << sum(1, 2)         << "\n";
    std::cout << "sum2(1.5, 2.5)   = " << sum2(1.5, 2.5)    << "\n";
    std::cout << "half(10)         = " << half(10)          << "\n";
    std::cout << "half(10.0)       = " << half(10.0)        << "\n";

    std::vector<int> v{1, 2, 3, 4};
    describe(v);

    // sum<std::vector<int>>(v, v);  // would fail with a concept-friendly diagnostic

    return 0;
}
