// std::get<T>(tuple) — C++14
// What: access a tuple element by its type, not its index.
// Why:  position-by-index is fragile when tuple shape changes; type access reads better
//       and breaks at compile time if there is ambiguity.
// Pre:  C++11 only allowed std::get<I>(t) by index.

#include <iostream>
#include <tuple>
#include <string>

int main() {
    std::tuple<int, std::string, double> t(42, std::string("hello"), 3.14);

    // C++11 way
    std::cout << "by index: " << std::get<0>(t) << " "
              << std::get<1>(t) << " " << std::get<2>(t) << "\n";

    // C++14 way — type-based
    std::cout << "by type:  " << std::get<int>(t) << " "
              << std::get<std::string>(t) << " "
              << std::get<double>(t) << "\n";

    // Ambiguity is rejected at compile time:
    // std::tuple<int, int> t2(1, 2);
    // std::get<int>(t2);  // error: more than one element of type int

    return 0;
}
