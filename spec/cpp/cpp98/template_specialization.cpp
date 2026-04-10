// Template specialization — C++98
// What: full and partial specialization let you provide a different implementation
//       for specific template arguments.
// Why:  the primary template is a default; specializations handle types or shapes
//       where the default is wrong, slow, or impossible.
// Pre:  there was no alternative — C++ has had templates since C++98.

#include <iostream>
#include <string>

// Primary template
template <typename T>
struct TypeName {
    static const char* get() { return "unknown"; }
};

// Full specialization for int
template <>
struct TypeName<int> {
    static const char* get() { return "int"; }
};

// Full specialization for std::string
template <>
struct TypeName<std::string> {
    static const char* get() { return "std::string"; }
};

// Primary template — pair of arbitrary types
template <typename A, typename B>
struct Pair {
    static void describe() { std::cout << "generic Pair\n"; }
};

// Partial specialization — both types are the same
template <typename T>
struct Pair<T, T> {
    static void describe() { std::cout << "Pair of identical types\n"; }
};

// Partial specialization — second type is a pointer
template <typename A, typename B>
struct Pair<A, B*> {
    static void describe() { std::cout << "Pair where second is a pointer\n"; }
};

int main() {
    std::cout << "TypeName<double>:      " << TypeName<double>::get()      << "\n";
    std::cout << "TypeName<int>:         " << TypeName<int>::get()         << "\n";
    std::cout << "TypeName<std::string>: " << TypeName<std::string>::get() << "\n";

    Pair<int, double>::describe();   // generic
    Pair<int, int>::describe();      // identical
    Pair<int, char*>::describe();    // pointer second

    return 0;
}
