// Variable templates — C++14
// What: a template that produces a variable, not a function or class.
// Why:  the natural way to express compile-time numeric constants per type, like pi<float>.
// Pre:  static constexpr members of a class template, or function templates returning a value.

#include <iostream>
#include <iomanip>

template <typename T>
constexpr T pi = T(3.1415926535897932385L);

template <typename T>
constexpr T e  = T(2.7182818284590452354L);

// Useful for type traits — same idea as std::is_integral_v in C++17
template <typename T>
constexpr bool is_pointer_v = false;

template <typename T>
constexpr bool is_pointer_v<T*> = true;   // partial specialization

int main() {
    std::cout << std::setprecision(20);
    std::cout << "pi<float>       = " << pi<float>       << "\n";
    std::cout << "pi<double>      = " << pi<double>      << "\n";
    std::cout << "pi<long double> = " << pi<long double> << "\n";
    std::cout << "e<double>       = " << e<double>       << "\n";

    std::cout << "is_pointer_v<int>    = " << is_pointer_v<int>    << "\n";
    std::cout << "is_pointer_v<int*>   = " << is_pointer_v<int*>   << "\n";
    std::cout << "is_pointer_v<char**> = " << is_pointer_v<char**> << "\n";

    return 0;
}
