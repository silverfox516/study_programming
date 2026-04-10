// Perfect forwarding — C++11
// What: a function template that takes T&& "universal references" and re-passes them with
//       std::forward<T>, preserving lvalue/rvalue category and constness.
// Why:  lets a wrapper (like std::make_unique or emplace_back) build an object in place
//       without unnecessary copies and without losing rvalue-ness.
// Pre:  wrappers needed multiple overloads for every combination of const/&/&&.

#include <iostream>
#include <utility>
#include <string>

// Track which overload was selected
void target(int&)        { std::cout << "target(int&)        — lvalue\n"; }
void target(const int&)  { std::cout << "target(const int&)  — const lvalue\n"; }
void target(int&&)       { std::cout << "target(int&&)       — rvalue\n"; }

// Universal reference + perfect forwarding
template <typename T>
void wrapper(T&& arg) {
    target(std::forward<T>(arg));
}

// A toy in-place factory
template <typename T, typename A1>
T* make(A1&& a1) {
    return new T(std::forward<A1>(a1));
}

class Greeter {
public:
    explicit Greeter(std::string name) : name_(std::move(name)) {
        std::cout << "Greeter built from '" << name_ << "'\n";
    }
private:
    std::string name_;
};

int main() {
    int       x  = 1;
    const int cx = 2;

    wrapper(x);           // -> target(int&)
    wrapper(cx);          // -> target(const int&)
    wrapper(3);           // -> target(int&&)
    wrapper(std::move(x));// -> target(int&&)

    // The string is moved all the way through wrapper -> Greeter constructor
    std::string s = "Alice";
    Greeter* g = make<Greeter>(std::move(s));
    delete g;

    return 0;
}
