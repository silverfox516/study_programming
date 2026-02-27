#include <iostream>
#include <expected>
#include <string>

enum class parse_error {
    invalid_input,
    overflow
};

std::expected<int, parse_error> parse_int(const std::string& s) {
    try {
        size_t pos;
        int result = std::stoi(s, &pos);
        if (pos != s.length()) {
            return std::unexpected(parse_error::invalid_input);
        }
        return result;
    } catch (const std::invalid_argument&) {
        return std::unexpected(parse_error::invalid_input);
    } catch (const std::out_of_range&) {
        return std::unexpected(parse_error::overflow);
    }
}

int main() {
    auto result1 = parse_int("123");
    if (result1) {
        std::cout << "Parsed: " << *result1 << std::endl;
    }

    auto result2 = parse_int("abc");
    if (!result2) {
        std::cout << "Failed to parse: " << static_cast<int>(result2.error()) << std::endl;
    }

    return 0;
}
