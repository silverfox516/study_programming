// Trailing return type — C++11
// What: write `auto f(args) -> ReturnType` instead of `ReturnType f(args)`.
// Why:  the return type can refer to parameters via decltype, which the leading-return
//       form cannot do because the parameters are not yet in scope.
// Pre:  return types depending on parameters required clumsy traits classes.

#include <iostream>
#include <vector>

// The return type depends on what (a + b) actually is — could be int, double, etc.
template <typename A, typename B>
auto add(A a, B b) -> decltype(a + b) {
    return a + b;
}

// Same idea — the return depends on dereferencing an iterator
template <typename Container>
auto first(Container& c) -> decltype(*c.begin()) {
    return *c.begin();
}

int main() {
    std::cout << "add(1, 2)      = " << add(1, 2)      << "\n";
    std::cout << "add(1, 2.5)    = " << add(1, 2.5)    << "\n";
    std::cout << "add(2.5f, 3.5f)= " << add(2.5f, 3.5f) << "\n";

    std::vector<int> v;
    v.push_back(10); v.push_back(20);
    std::cout << "first(v) = " << first(v) << "\n";

    return 0;
}
