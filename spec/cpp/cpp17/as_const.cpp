// C++17: std::as_const — adds const to an lvalue reference

#include <iostream>
#include <utility>
#include <vector>
#include <string>

// Overload set that behaves differently for const vs non-const
void process(std::string& s) {
    std::cout << "  non-const: modifying \"" << s << "\"\n";
    s += "!";
}

void process(const std::string& s) {
    std::cout << "  const: reading \"" << s << "\"\n";
}

int main() {
    std::string msg = "hello";

    // Without as_const: calls non-const overload
    std::cout << "Direct call:\n";
    process(msg);

    // With as_const: forces const overload
    std::cout << "as_const call:\n";
    process(std::as_const(msg));

    // Useful with range-for to avoid triggering COW or non-const operator[]
    std::vector<int> v{1, 2, 3, 4, 5};

    // This calls non-const begin/end
    for (auto& x : v) { (void)x; }

    // This calls const begin/end — no accidental modification
    for (auto& x : std::as_const(v)) {
        // x is const int& — can't modify
        std::cout << x << " ";
    }
    std::cout << "\n";

    // Practical: force const iterator
    auto it = std::as_const(v).begin();  // const_iterator
    std::cout << "First element (const_iterator): " << *it << "\n";

    return 0;
}
