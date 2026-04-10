// <charconv> — C++17
// What: std::from_chars / std::to_chars — fast, locale-independent number<->text conversion.
// Why:  std::stoi/sprintf/streams are locale-aware (slow) and allocate (slower).
//       charconv beats them all and is the right tool for parsers and serializers.
// Pre:  atoi/strtol with no error reporting, or sprintf/streams with locale overhead.

#include <iostream>
#include <charconv>
#include <string>
#include <system_error>

int main() {
    // Parse: from_chars
    std::string text = "12345 not-a-number 678";
    int value = 0;
    auto [ptr, ec] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (ec == std::errc{}) {
        std::cout << "parsed " << value << ", stopped at offset " << (ptr - text.data()) << "\n";
    }

    // Detect failure cleanly
    std::string bad = "abc";
    int v2 = 0;
    auto r2 = std::from_chars(bad.data(), bad.data() + bad.size(), v2);
    if (r2.ec == std::errc::invalid_argument) {
        std::cout << "rejected '" << bad << "' as not a number\n";
    }

    // Format: to_chars
    char    buffer[32];
    int     n = 42;
    auto    res = std::to_chars(buffer, buffer + sizeof(buffer), n);
    if (res.ec == std::errc{}) {
        std::cout << "formatted: " << std::string(buffer, res.ptr) << "\n";
    }

    // Hex base
    auto res2 = std::to_chars(buffer, buffer + sizeof(buffer), 255, 16);
    std::cout << "255 in hex: " << std::string(buffer, res2.ptr) << "\n";

    return 0;
}
