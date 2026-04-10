// Deducing this (explicit object parameter) — C++23
// What: a member function can take its `this` as a named, deducible parameter
//       written in the parameter list: `void f(this Self& self, ...)`.
// Why:  removes hours of duplication: `T& foo() &; const T& foo() const&; T&& foo() &&;`
//       collapses into one template that handles all four ref/const variants.
//       Also enables CRTP-without-CRTP and recursion-via-`this`.
// Pre:  multiple ref-qualified overloads or true CRTP base classes.

#include <iostream>
#include <vector>
#include <string>

class Container {
public:
    Container() : data_{1, 2, 3, 4, 5} {}

    // ONE function, deduces lvalue/rvalue/const automatically
    template <typename Self>
    auto&& at(this Self&& self, std::size_t i) {
        return std::forward<Self>(self).data_[i];
    }

private:
    std::vector<int> data_;
};

// CRTP-without-CRTP: a base class that uses Self to call derived members
struct Comparable {
    template <typename Self>
    bool less_than(this const Self& self, const Self& other) {
        return self.value() < other.value();
    }
};

struct Number : Comparable {
    int v;
    explicit Number(int x) : v(x) {}
    int value() const { return v; }
};

int main() {
    Container c;
    std::cout << "c.at(0) = " << c.at(0) << "\n";

    // Modify through the deduced reference
    c.at(2) = 99;
    std::cout << "after assign, c.at(2) = " << c.at(2) << "\n";

    // const access — same function, deduced as const
    const Container& cc = c;
    std::cout << "cc.at(2) = " << cc.at(2) << "\n";

    // The Comparable mixin works without ever templating Number
    Number a(3), b(7);
    std::cout << "a < b: " << a.less_than(b) << "\n";

    return 0;
}
