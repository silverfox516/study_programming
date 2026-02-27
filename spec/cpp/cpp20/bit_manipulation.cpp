#include <iostream>
#include <bit>
#include <cstdint>
#include <iomanip>

struct MyStruct {
    char c;
    int i;
};

int main() {
    // std::bit_cast (safe constant-time bitwise reinterpretation)
    float f = 3.14159f;
    uint32_t f_as_int = std::bit_cast<uint32_t>(f);
    
    std::cout << "Float: " << f << std::endl;
    std::cout << "Bits: 0x" << std::hex << f_as_int << std::dec << std::endl;

    // std::popcount (population count - number of set bits)
    uint8_t u = 0b10110010;
    std::cout << "Popcount of " << (int)u << ": " << std::popcount(u) << std::endl;

    // std::countl_zero (count leading zeros)
    std::cout << "Leading zeros in 8-bit integer: " << std::countl_zero(u) << std::endl;

    // Endianness
    if constexpr (std::endian::native == std::endian::little) {
        std::cout << "Little-endian system" << std::endl;
    } else {
        std::cout << "Big-endian system" << std::endl;
    }

    return 0;
}
