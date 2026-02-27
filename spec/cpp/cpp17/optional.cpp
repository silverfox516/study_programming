#include <iostream>
#include <optional>

std::optional<int> to_int(const std::string& s) {
    try {
        return std::stoi(s);
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

int main() {
    auto opt1 = to_int("123");
    if (opt1) {
        std::cout << "Value: " << *opt1 << std::endl;
    }

    auto opt2 = to_int("abc");
    if (!opt2) {
        std::cout << "Conversion failed" << std::endl;
    }

    std::cout << "Value or default: " << opt2.value_or(0) << std::endl;

    return 0;
}
