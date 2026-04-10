// Parenthesized aggregate initialization — C++20
// What: aggregates can now be initialized with `()` (parentheses), like classes
//       with constructors. Removes a long-standing inconsistency.
// Why:  `T(args...)` worked for classes but failed for aggregates, breaking
//       std::make_shared<Aggregate>(args...) and similar generic factories.
// Pre:  you had to write `T{args...}` for aggregates, which sometimes selected
//       a different constructor (initializer_list) than intended.

#include <iostream>
#include <memory>

struct Point {
    int x;
    int y;
};

template <typename T, typename... Args>
T make(Args&&... args) {
    return T(std::forward<Args>(args)...);   // works for both classes and aggregates now
}

int main() {
    // C++20: parentheses initialize the aggregate directly
    Point p(1, 2);
    std::cout << "p = " << p.x << "," << p.y << "\n";

    // Pre-C++20 this required braces
    Point q{3, 4};
    std::cout << "q = " << q.x << "," << q.y << "\n";

    // The big payoff: generic factories work uniformly
    auto r = make<Point>(5, 6);
    std::cout << "r = " << r.x << "," << r.y << "\n";

    // make_shared on an aggregate now works
    auto sp = std::make_shared<Point>(7, 8);
    std::cout << "*sp = " << sp->x << "," << sp->y << "\n";

    return 0;
}
