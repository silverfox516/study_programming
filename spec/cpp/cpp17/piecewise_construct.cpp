// std::piecewise_construct — actually C++11, but its main use case (map::try_emplace,
// emplace with non-copyable types) became natural in C++17.
// What: a tag that tells std::pair to construct its first/second members from two
//       separate tuples of constructor arguments, in place.
// Why:  necessary when the pair members can't be copied or moved into pair, only
//       constructed in place.
// Pre:  no clean way to in-place construct both halves of a pair.

#include <iostream>
#include <tuple>
#include <utility>
#include <map>
#include <string>

class NoCopyMove {
public:
    NoCopyMove(int a, int b) : sum_(a + b) { std::cout << "NoCopyMove(" << a << "," << b << ")\n"; }
    NoCopyMove(const NoCopyMove&)            = delete;
    NoCopyMove(NoCopyMove&&)                 = delete;
    NoCopyMove& operator=(const NoCopyMove&) = delete;
    int value() const { return sum_; }
private:
    int sum_;
};

int main() {
    // Construct the pair in place — neither member is copied or moved
    std::pair<std::string, NoCopyMove> p(
        std::piecewise_construct,
        std::forward_as_tuple("answer"),
        std::forward_as_tuple(20, 22));

    std::cout << "first  = " << p.first        << "\n";
    std::cout << "second = " << p.second.value() << "\n";

    // The natural map equivalent in C++17 is try_emplace, which does the same thing
    std::map<std::string, NoCopyMove> m;
    m.try_emplace("life", 21, 21);
    std::cout << "map['life'] = " << m.at("life").value() << "\n";

    return 0;
}
