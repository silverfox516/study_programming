// static operator() — C++23
// What: operator() in a class can be marked `static`, removing the implicit `this`
//       parameter when the class doesn't need state.
// Why:  function-object types used as algorithm comparators or hash functors had to
//       waste a slot for `this` even when stateless. Now they don't.
// Pre:  the implicit `this` parameter was unavoidable, so a stateless functor still
//       paid the cost of a hidden pointer in its calling convention.

#include <iostream>
#include <algorithm>
#include <vector>

struct Square {
    static int operator()(int n) { return n * n; }
};

struct Greater {
    static bool operator()(int a, int b) { return a > b; }
};

int main() {
    Square sq;
    // Both forms work — call via instance or via the type
    std::cout << "sq(5)         = " << sq(5)         << "\n";
    std::cout << "Square{}(7)   = " << Square{}(7)   << "\n";
    std::cout << "Square()(9)   = " << Square()(9)   << "\n";

    // Pass as a comparator
    std::vector<int> v{3, 1, 4, 1, 5, 9, 2, 6};
    std::sort(v.begin(), v.end(), Greater{});
    for (int n : v) std::cout << n << " ";
    std::cout << "\n";

    return 0;
}
