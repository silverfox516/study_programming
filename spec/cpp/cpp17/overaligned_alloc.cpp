// C++17: over-aligned dynamic allocation — operator new respects alignas

#include <iostream>
#include <cstddef>
#include <memory>
#include <new>

// Over-aligned type (e.g., for SIMD)
struct alignas(64) CacheLine {
    char data[64];
};

struct alignas(256) BigAligned {
    double values[4];
};

int main() {
    // C++17: new automatically respects alignas > __STDCPP_DEFAULT_NEW_ALIGNMENT__
    auto* cl = new CacheLine{};
    std::cout << "CacheLine alignment: " << alignof(CacheLine) << "\n";
    std::cout << "CacheLine address:   " << cl << "\n";
    std::cout << "  aligned to 64? " << (reinterpret_cast<std::uintptr_t>(cl) % 64 == 0
                                          ? "yes" : "no") << "\n";

    auto* ba = new BigAligned{};
    std::cout << "\nBigAligned alignment: " << alignof(BigAligned) << "\n";
    std::cout << "BigAligned address:   " << ba << "\n";
    std::cout << "  aligned to 256? " << (reinterpret_cast<std::uintptr_t>(ba) % 256 == 0
                                           ? "yes" : "no") << "\n";

    // unique_ptr works with over-aligned types too
    auto uptr = std::make_unique<CacheLine>();
    std::cout << "\nunique_ptr<CacheLine> address: " << uptr.get() << "\n";
    std::cout << "  aligned to 64? " << (reinterpret_cast<std::uintptr_t>(uptr.get()) % 64 == 0
                                          ? "yes" : "no") << "\n";

    // Default new alignment
    std::cout << "\n__STDCPP_DEFAULT_NEW_ALIGNMENT__: "
              << __STDCPP_DEFAULT_NEW_ALIGNMENT__ << "\n";

    delete cl;
    delete ba;

    return 0;
}
