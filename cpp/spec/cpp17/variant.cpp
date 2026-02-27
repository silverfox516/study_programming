#include <iostream>
#include <variant>
#include <string>

int main() {
    std::variant<int, std::string> v = "hello";
    std::cout << std::get<std::string>(v) << std::endl;

    v = 123;
    std::cout << std::get<int>(v) << std::endl;

    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>)
            std::cout << "is int: " << arg << std::endl;
        else if constexpr (std::is_same_v<T, std::string>)
            std::cout << "is string: " << arg << std::endl;
    }, v);

    return 0;
}
