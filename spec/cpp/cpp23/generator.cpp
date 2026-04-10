// std::generator — C++23
// What: a coroutine-based lazy sequence type. Library-supplied alternative to
//       hand-rolling the Generator promise/handle machinery.
// Why:  std::generator is the standard answer to "I want a Python-style generator".
// Pre:  no language coroutines (pre-C++20); hand-written Generator types (C++20).
// See also: spec/cpp/cpp20/coroutine_generator.cpp shows the manual version.
//
// libc++ feature test: __cpp_lib_generator (Apple clang 21 reports this NOT defined)

#include <iostream>
#include <version>

#if defined(__cpp_lib_generator) && __cpp_lib_generator >= 202207L
#  include <generator>
#  define HAVE_GENERATOR 1
#else
#  define HAVE_GENERATOR 0
#endif

#if HAVE_GENERATOR
std::generator<int> first_n_squares(int n) {
    for (int i = 1; i <= n; ++i) co_yield i * i;
}

std::generator<int> fibonacci() {
    int a = 0, b = 1;
    while (true) {
        co_yield a;
        int next = a + b;
        a = b;
        b = next;
    }
}
#endif

int main() {
#if HAVE_GENERATOR
    for (int x : first_n_squares(5)) std::cout << x << " ";
    std::cout << "\n";

    int count = 0;
    for (int x : fibonacci()) {
        if (count++ == 10) break;
        std::cout << x << " ";
    }
    std::cout << "\n";
#else
    std::cout << "<generator> not available in this libc++ build.\n";
    std::cout << "See spec/cpp/cpp20/coroutine_generator.cpp for the hand-rolled version.\n";
#endif
    return 0;
}
