#include <iostream>
#include <functional>

int calculate(int a, int b, int c) {
    return a * b + c;
}

int main() {
    // std::bind_front binds the first N arguments.
    // Unlike std::bind, it preserves perfect forwarding, 
    // doesn't silently swallow extra arguments, and avoids placeholders.
    auto multiply_by_2_then_add = std::bind_front(calculate, 2);
    
    std::cout << "2 * 3 + 4 = " << multiply_by_2_then_add(3, 4) << std::endl;

    auto multiply_2_and_3_add = std::bind_front(calculate, 2, 3);
    std::cout << "2 * 3 + 5 = " << multiply_2_and_3_add(5) << std::endl;

    return 0;
}
