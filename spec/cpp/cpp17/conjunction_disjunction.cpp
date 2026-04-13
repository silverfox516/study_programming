// C++17: std::conjunction, std::disjunction, std::negation
// — variadic logical meta-functions with short-circuit evaluation

#include <iostream>
#include <type_traits>
#include <string>
#include <vector>

// Check if all types are integral
template <typename... Ts>
constexpr bool all_integral_v = std::conjunction_v<std::is_integral<Ts>...>;

// Check if any type is floating point
template <typename... Ts>
constexpr bool any_floating_v = std::disjunction_v<std::is_floating_point<Ts>...>;

// Check if type is NOT a pointer
template <typename T>
constexpr bool not_pointer_v = std::negation_v<std::is_pointer<T>>;

// SFINAE with conjunction — only enable if all args are arithmetic
template <typename... Ts>
std::enable_if_t<std::conjunction_v<std::is_arithmetic<Ts>...>, double>
sum(Ts... args) {
    return (static_cast<double>(args) + ...);
}

// Short-circuit: disjunction stops at first true
// (doesn't instantiate remaining traits)
template <typename T>
struct AlwaysFalse : std::false_type {};

int main() {
    // conjunction: all must be true
    std::cout << std::boolalpha;
    std::cout << "all_integral<int,long,char>: "
              << all_integral_v<int, long, char> << "\n";      // true
    std::cout << "all_integral<int,double>: "
              << all_integral_v<int, double> << "\n";            // false

    // disjunction: any must be true
    std::cout << "any_floating<int,float>: "
              << any_floating_v<int, float> << "\n";             // true
    std::cout << "any_floating<int,long>: "
              << any_floating_v<int, long> << "\n";              // false

    // negation
    std::cout << "not_pointer<int>: " << not_pointer_v<int> << "\n";    // true
    std::cout << "not_pointer<int*>: " << not_pointer_v<int*> << "\n";  // false

    // practical use: SFINAE
    std::cout << "sum(1, 2.5, 3L) = " << sum(1, 2.5, 3L) << "\n";
    // sum("hello", 1);  // compile error — string is not arithmetic

    return 0;
}
