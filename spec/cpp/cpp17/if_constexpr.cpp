#include <iostream>
#include <type_traits>

template <typename T>
auto get_value(T t) {
    if constexpr (std::is_pointer_v<T>) {
        return *t;
    } else {
        return t;
    }
}

int main() {
    int x = 5;
    int* p = &x;

    std::cout << get_value(x) << std::endl;
    std::cout << get_value(p) << std::endl;

    return 0;
}
