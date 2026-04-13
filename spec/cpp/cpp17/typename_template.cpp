// C++17: typename in template template parameters
// — 'typename' can be used where only 'class' was allowed before

#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include <string>

// Before C++17: only 'class' was valid here
// template <template <class, class> class Container>

// C++17: 'typename' is now allowed in template template parameters
template <template <typename, typename> typename Container>
struct Wrapper {
    Container<int, std::allocator<int>> data;

    void add(int v) { data.push_back(v); }
    void print() const {
        for (auto x : data) std::cout << x << " ";
        std::cout << "\n";
    }
};

// Also works with auto non-type parameters (C++17)
template <auto Value>
struct Constant {
    static constexpr auto value = Value;
};

int main() {
    // template template with typename keyword
    Wrapper<std::vector> wv;
    wv.add(1); wv.add(2); wv.add(3);
    std::cout << "vector: ";
    wv.print();

    Wrapper<std::list> wl;
    wl.add(10); wl.add(20);
    std::cout << "list:   ";
    wl.print();

    Wrapper<std::deque> wd;
    wd.add(100); wd.add(200);
    std::cout << "deque:  ";
    wd.print();

    // auto non-type template param
    std::cout << "Constant<42>: " << Constant<42>::value << "\n";
    std::cout << "Constant<'A'>: " << Constant<'A'>::value << "\n";
    std::cout << "Constant<true>: " << Constant<true>::value << "\n";

    return 0;
}
