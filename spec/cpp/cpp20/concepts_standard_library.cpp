// <concepts> standard library — C++20
// What: a catalog of pre-defined concepts in <concepts>: same_as, derived_from,
//       integral, floating_point, equality_comparable, totally_ordered, etc.
// Why:  spares you from re-defining the obvious predicates everyone needs.
// Pre:  re-implementing each as a custom enable_if helper or boolean trait.

#include <iostream>
#include <concepts>
#include <string>

// std::same_as constrains exact identity (modulo cv-ref)
template <std::same_as<int> T>
void only_int(T) { std::cout << "only_int called\n"; }

// std::derived_from constrains a class hierarchy
struct Animal {};
struct Dog : Animal {};
struct Rock {};

template <std::derived_from<Animal> T>
void pet(T&) { std::cout << "petted an animal\n"; }

// std::equality_comparable lets you write generic == users
template <std::equality_comparable T>
bool same(const T& a, const T& b) { return a == b; }

// std::totally_ordered for sortable types
template <std::totally_ordered T>
T larger(T a, T b) { return a < b ? b : a; }

int main() {
    only_int(42);
    // only_int(42L);  // would fail — long is not the same_as int

    Dog d; pet(d);
    // Rock r; pet(r);  // would fail — Rock is not derived_from Animal

    std::cout << "same(1, 1)        = " << same(1, 1)               << "\n";
    std::cout << "same(\"hi\", \"hi\") = " << same(std::string("hi"), std::string("hi")) << "\n";
    std::cout << "larger(3, 7)      = " << larger(3, 7)             << "\n";

    // Concepts are just compile-time bools — you can query them directly
    std::cout << "integral<int>:           " << std::integral<int>           << "\n";
    std::cout << "floating_point<int>:     " << std::floating_point<int>     << "\n";
    std::cout << "regular<std::string>:    " << std::regular<std::string>    << "\n";

    return 0;
}
