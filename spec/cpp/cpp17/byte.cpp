// std::byte — C++17
// What: a distinct enum-class type representing a raw byte, neither character nor integer.
// Why:  signals "this is binary data, not text and not arithmetic" to readers and the compiler.
// Pre:  uint8_t / char / unsigned char — all overloaded with text/integer meaning.

#include <iostream>
#include <cstddef>

int main() {
    std::byte b{0xAB};

    // Bitwise operators are defined; arithmetic is intentionally not
    std::byte mask = std::byte{0x0F};
    std::byte low  = b & mask;
    std::byte high = b >> 4;

    // To inspect or print, convert explicitly
    std::cout << std::hex
              << "byte:      0x" << std::to_integer<int>(b)    << "\n"
              << "low nib:   0x" << std::to_integer<int>(low)  << "\n"
              << "high nib:  0x" << std::to_integer<int>(high) << "\n";

    // The compiler rejects accidental arithmetic:
    // std::byte sum = b + std::byte{1};  // error: no operator+

    return 0;
}
