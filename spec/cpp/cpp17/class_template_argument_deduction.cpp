// Class template argument deduction (CTAD) — C++17
// What: the compiler deduces a class template's parameters from its constructor arguments,
//       so you can write `std::pair p{1, 2.0}` instead of `std::pair<int, double> p{1, 2.0}`.
// Why:  removes redundant type names that the compiler can already see.
// Pre:  required factory functions like std::make_pair / std::make_tuple to deduce.

#include <iostream>
#include <vector>
#include <tuple>
#include <string>

template <typename T>
class Box {
public:
    Box(T v) : value_(v) {}
    void show() const { std::cout << value_ << "\n"; }
private:
    T value_;
};

// User-defined deduction guide (only needed when the compiler can't figure it out)
template <typename Iter>
class Range {
public:
    Range(Iter b, Iter e) : begin_(b), end_(e) {}
    Iter begin() const { return begin_; }
    Iter end()   const { return end_; }
private:
    Iter begin_, end_;
};
template <typename Iter> Range(Iter, Iter) -> Range<Iter>;

int main() {
    // Standard library types
    std::pair  p{1, 2.5};                       // pair<int, double>
    std::tuple t{42, std::string("hi"), 3.14};  // tuple<int, string, double>
    std::vector v{10, 20, 30};                  // vector<int>

    std::cout << "pair:   " << p.first << " " << p.second << "\n";
    std::cout << "tuple:  " << std::get<0>(t) << " " << std::get<1>(t) << "\n";
    std::cout << "vector: " << v.size() << " elements\n";

    // User-defined class — CTAD picks T = const char*
    Box b1{"hello"};
    Box b2{42};
    b1.show();
    b2.show();

    // Deduction guide kicks in for the iterator pair
    int data[] = {1, 2, 3, 4};
    Range r{std::begin(data), std::end(data)};
    for (auto x : r) std::cout << x << " ";
    std::cout << "\n";

    return 0;
}
