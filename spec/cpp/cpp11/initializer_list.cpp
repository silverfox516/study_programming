// std::initializer_list — C++11
// What: a lightweight proxy over a brace-enclosed list of values, accepted by special constructors.
// Why:  lets a class be constructed from {a, b, c} just like the built-in arrays and std::vector.
// Pre:  needed N overloaded constructors or a vector argument; brace-init was not available.

#include <iostream>
#include <initializer_list>
#include <vector>

class IntBag {
public:
    IntBag(std::initializer_list<int> values) : data_(values) {}

    void print() const {
        for (std::vector<int>::const_iterator it = data_.begin(); it != data_.end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << "\n";
    }

private:
    std::vector<int> data_;
};

// initializer_list also works as a free function parameter
int sum(std::initializer_list<int> list) {
    int total = 0;
    for (auto v : list) total += v;
    return total;
}

int main() {
    IntBag bag = {1, 2, 3, 4, 5};
    bag.print();

    // Standard library types use the same mechanism
    std::vector<int> v = {10, 20, 30};
    std::cout << "vector size: " << v.size() << "\n";

    std::cout << "sum({1,2,3,4}) = " << sum({1, 2, 3, 4}) << "\n";

    return 0;
}
