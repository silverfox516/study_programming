#include <iostream>
#include <tuple>
#include <map>

int main() {
    std::tuple<int, double, std::string> t = {1, 3.14, "hello"};
    auto [a, b, c] = t;
    std::cout << a << ", " << b << ", " << c << std::endl;

    std::map<std::string, int> map = {{"a", 1}, {"b", 2}};
    for (const auto& [key, value] : map) {
        std::cout << key << ": " << value << std::endl;
    }

    return 0;
}
