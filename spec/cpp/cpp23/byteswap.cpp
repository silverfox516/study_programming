#include <iostream>
#include <bit>
#include <cstdint>
#include <iomanip>

int main() {
    uint32_t u = 0x12345678;
    
    uint32_t swapped = std::byteswap(u);
    
    std::cout << "Original: 0x" << std::hex << u << std::endl;
    std::cout << "Swapped:  0x" << std::hex << swapped << std::endl;
    
    // Check if reversible
    if (std::byteswap(swapped) == u) {
        std::cout << "Reversing swap restores original value." << std::endl;
    }

    return 0;
}
