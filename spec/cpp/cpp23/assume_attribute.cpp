// [[assume(...)]] — C++23
// What: an attribute that promises a condition is true at this point in execution.
//       The compiler may use the assumption to optimize, but does not check it.
// Why:  lets you communicate invariants the optimizer cannot derive, without
//       runtime cost. Like `__builtin_assume` finally standardized.
// Pre:  vendor extensions (__assume on MSVC, __builtin_assume on clang).
// WARNING: a violated assumption is undefined behavior. Use sparingly.

#include <iostream>

int divide(int x, int n) {
    [[assume(n > 0)]];     // tell the optimizer n is always positive
    return x / n;
}

int safe_index(int* arr, int i, int len) {
    [[assume(i >= 0 && i < len)]];   // promise the index is in range
    return arr[i];
}

int main() {
    int arr[] = {10, 20, 30, 40, 50};
    std::cout << "divide(100, 4) = " << divide(100, 4) << "\n";
    std::cout << "safe_index(arr, 2, 5) = " << safe_index(arr, 2, 5) << "\n";

    // Calling divide(100, 0) here would be UB, not a runtime error.
    return 0;
}
