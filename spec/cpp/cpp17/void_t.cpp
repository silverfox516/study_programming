// C++17: std::void_t — SFINAE helper for detecting valid expressions

#include <iostream>
#include <type_traits>
#include <vector>
#include <string>
#include <map>

// Detect if T has a .size() method
template <typename, typename = std::void_t<>>
struct has_size : std::false_type {};

template <typename T>
struct has_size<T, std::void_t<decltype(std::declval<T>().size())>>
    : std::true_type {};

// Detect if T has begin/end (is iterable)
template <typename, typename = std::void_t<>>
struct is_iterable : std::false_type {};

template <typename T>
struct is_iterable<T, std::void_t<
    decltype(std::declval<T>().begin()),
    decltype(std::declval<T>().end())>>
    : std::true_type {};

// Detect if T has operator[]
template <typename, typename = std::void_t<>>
struct has_subscript : std::false_type {};

template <typename T>
struct has_subscript<T, std::void_t<
    decltype(std::declval<T>()[std::declval<typename T::size_type>()])>>
    : std::true_type {};

// Detect nested type alias
template <typename, typename = std::void_t<>>
struct has_value_type : std::false_type {};

template <typename T>
struct has_value_type<T, std::void_t<typename T::value_type>>
    : std::true_type {};

int main() {
    std::cout << std::boolalpha;

    // has_size
    std::cout << "vector has size: " << has_size<std::vector<int>>::value << "\n";
    std::cout << "int has size:    " << has_size<int>::value << "\n";

    // is_iterable
    std::cout << "string iterable: " << is_iterable<std::string>::value << "\n";
    std::cout << "double iterable: " << is_iterable<double>::value << "\n";

    // has_subscript
    std::cout << "vector has []:   " << has_subscript<std::vector<int>>::value << "\n";

    // has_value_type
    std::cout << "map has value_type:  " << has_value_type<std::map<int,int>>::value << "\n";
    std::cout << "float has value_type: " << has_value_type<float>::value << "\n";

    return 0;
}
