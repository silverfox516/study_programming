// <type_traits> — C++11
// What: compile-time type queries and transformations (is_integral, remove_reference, etc.).
// Why:  enables generic code to inspect and adapt to its template arguments.
// Pre:  hand-written SFINAE tricks per project; no portable way to ask "is this an integer?".

#include <iostream>
#include <type_traits>
#include <string>

// Compile-time inspection
template <typename T>
void describe() {
    std::cout << "is_integral:        " << std::is_integral<T>::value        << "\n";
    std::cout << "is_floating_point:  " << std::is_floating_point<T>::value  << "\n";
    std::cout << "is_pointer:         " << std::is_pointer<T>::value         << "\n";
    std::cout << "is_reference:       " << std::is_reference<T>::value       << "\n";
    std::cout << "is_class:           " << std::is_class<T>::value           << "\n";
}

// SFINAE: enable a function only for arithmetic types
template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, T>::type
twice(T value) {
    return value + value;
}

// Type transformation: strip reference and cv-qualifiers
template <typename T>
void show_decayed() {
    typedef typename std::remove_cv<
        typename std::remove_reference<T>::type
    >::type Bare;
    std::cout << "is_same<Bare, int>: " << std::is_same<Bare, int>::value << "\n";
}

int main() {
    std::cout << "--- describe<int> ---\n";        describe<int>();
    std::cout << "--- describe<double*> ---\n";    describe<double*>();
    std::cout << "--- describe<std::string> ---\n"; describe<std::string>();

    std::cout << "twice(21)   = " << twice(21)   << "\n";
    std::cout << "twice(2.5)  = " << twice(2.5)  << "\n";
    // twice("hi")  // would fail to compile — not arithmetic

    std::cout << "decayed const int& vs int: ";
    show_decayed<const int&>();

    return 0;
}
