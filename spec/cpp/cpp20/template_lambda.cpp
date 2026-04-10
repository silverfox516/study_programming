// Template lambdas — C++20
// What: lambdas can take an explicit template parameter list with `[]<typename T>(...)`.
// Why:  C++14 generic lambdas used `auto`, but you couldn't name the deduced type
//       to use it in the body (e.g. for sizeof, std::vector<T>, decay, etc.).
// Pre:  generic lambdas with `auto` parameters and clumsy decltype workarounds.

#include <iostream>
#include <vector>
#include <type_traits>

int main() {
    // Name the parameter type so we can use it explicitly in the body
    auto print_type_size = []<typename T>(T value) {
        std::cout << "sizeof(T) = " << sizeof(T)
                  << ", value = " << value << "\n";
    };
    print_type_size(42);
    print_type_size(3.14);
    print_type_size('A');

    // Constrain the template parameter with a concept
    auto add_integral = []<std::integral T>(T a, T b) { return a + b; };
    std::cout << "add_integral(1,2) = " << add_integral(1, 2) << "\n";
    // add_integral(1.0, 2.0);  // would fail to compile — T must be integral

    // Decompose container types — useful for traits-based work
    auto first_value_type = []<typename T>(const std::vector<T>&) -> T {
        return T{};
    };
    std::vector<int> v{1, 2, 3};
    auto x = first_value_type(v);
    std::cout << "first_value_type(v) returned " << x
              << " (typed as int = " << std::is_same_v<decltype(x), int> << ")\n";

    return 0;
}
