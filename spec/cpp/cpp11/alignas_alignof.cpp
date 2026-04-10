// alignas / alignof — C++11
// What: alignof(T) queries the alignment a type requires;
//       alignas(N) forces a variable or member to be aligned to N bytes.
// Why:  needed for SIMD types, cache-line padding, and hardware-specific layouts.
// Pre:  vendor extensions (__attribute__((aligned(N))), __declspec(align(N))).

#include <iostream>
#include <cstdint>

struct alignas(64) CacheLinePadded {
    int value;
};

struct alignas(16) Vec4 {
    float x, y, z, w;
};

int main() {
    std::cout << "alignof(char):       " << alignof(char)       << "\n";
    std::cout << "alignof(int):        " << alignof(int)        << "\n";
    std::cout << "alignof(double):     " << alignof(double)     << "\n";
    std::cout << "alignof(std::max_align_t): " << alignof(std::max_align_t) << "\n";

    CacheLinePadded c{};
    std::cout << "alignof(CacheLinePadded): " << alignof(CacheLinePadded) << "\n";
    std::cout << "alignof(Vec4):            " << alignof(Vec4)            << "\n";

    // Address of a CacheLinePadded must be a multiple of 64
    std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(&c);
    std::cout << "addr % 64 = " << (addr % 64) << " (should be 0)\n";

    // Local alignas on a buffer big enough for any Vec4
    alignas(Vec4) unsigned char storage[sizeof(Vec4)];
    Vec4* placed = new (storage) Vec4{1.f, 2.f, 3.f, 4.f};
    std::cout << "placed Vec4: " << placed->x << "," << placed->y << "\n";

    return 0;
}
