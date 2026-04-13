// C++17: user-defined deduction guides for class template argument deduction

#include <iostream>
#include <string>
#include <vector>

// Custom container with deduction guide
template <typename T>
struct MyContainer {
    T value;
    explicit MyContainer(T v) : value(std::move(v)) {}
};

// Deduction guide: const char* → std::string
MyContainer(const char*) -> MyContainer<std::string>;

// Pair-like type with deduction guide
template <typename T, typename U>
struct Pair {
    T first;
    U second;
};

// Aggregate deduction guide
template <typename T, typename U>
Pair(T, U) -> Pair<T, U>;

// Range-based deduction guide
template <typename Iter>
struct Range {
    Iter begin_;
    Iter end_;
    Range(Iter b, Iter e) : begin_(b), end_(e) {}
};

template <typename Iter>
Range(Iter, Iter) -> Range<Iter>;

int main() {
    // Without deduction guide: MyContainer<const char*>
    // With deduction guide: MyContainer<std::string>
    MyContainer c1("hello");
    std::cout << "c1: " << c1.value << " (string)\n";

    // Regular CTAD still works
    MyContainer c2(42);
    std::cout << "c2: " << c2.value << " (int)\n";

    // Pair deduction
    Pair p{3.14, std::string("pi")};
    std::cout << "Pair: " << p.first << ", " << p.second << "\n";

    // Range deduction
    std::vector<int> v{1, 2, 3, 4, 5};
    Range r(v.begin(), v.end());
    std::cout << "Range size: " << std::distance(r.begin_, r.end_) << "\n";

    return 0;
}
