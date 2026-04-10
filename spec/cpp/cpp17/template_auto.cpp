// template<auto V> — C++17
// What: a non-type template parameter whose type is deduced from the argument.
// Why:  before C++17 you had to write `template<int V>`, `template<long V>`, etc., and
//       a generic "value parameter" required a separate type parameter as well.
// Pre:  template<typename T, T V>, written everywhere a value template was needed.

#include <iostream>

template <auto V>
struct Constant {
    static constexpr auto value = V;
};

// Different non-type parameter types are inferred separately
template <auto N>
auto multiply(int x) {
    return x * N;
}

// Pointer-to-member as a non-type parameter
struct Widget { int data = 100; };

template <auto MemberPtr>
auto get_member(const Widget& w) {
    return w.*MemberPtr;
}

int main() {
    std::cout << "Constant<42>::value     = " << Constant<42>::value     << "\n";
    std::cout << "Constant<'X'>::value    = " << Constant<'X'>::value    << "\n";
    // Note: floating-point non-type template parameters are C++20, not C++17.

    std::cout << "multiply<3>(10)         = " << multiply<3>(10)         << "\n";
    std::cout << "multiply<5L>(10)        = " << multiply<5L>(10)        << "\n";

    Widget w;
    std::cout << "get_member<&Widget::data>(w) = "
              << get_member<&Widget::data>(w) << "\n";

    return 0;
}
