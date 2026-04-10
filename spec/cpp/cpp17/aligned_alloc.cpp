// Over-aligned new — C++17
// What: when you `new` a type whose alignment exceeds __STDCPP_DEFAULT_NEW_ALIGNMENT__,
//       the compiler picks aligned operator new[]/delete[] automatically.
// Why:  before C++17, allocating SIMD types with new could return unaligned memory.
// Pre:  posix_memalign / _aligned_malloc / placement new on aligned buffers.

#include <iostream>
#include <cstdint>
#include <new>

struct alignas(64) CacheLineThing {
    int data;
};

int main() {
    std::cout << "default new alignment: "
              << __STDCPP_DEFAULT_NEW_ALIGNMENT__ << "\n";

    // C++17: this picks the aligned operator new automatically
    CacheLineThing* p = new CacheLineThing;
    std::uintptr_t  addr = reinterpret_cast<std::uintptr_t>(p);
    std::cout << "addr: 0x" << std::hex << addr << std::dec
              << " (mod 64 = " << (addr % 64) << ")\n";

    delete p;

    // Explicit aligned alloc via std::align_val_t
    void* raw = ::operator new(64, std::align_val_t{64});
    std::cout << "explicit: " << ((reinterpret_cast<std::uintptr_t>(raw) % 64 == 0)
                                  ? "aligned" : "NOT aligned") << "\n";
    ::operator delete(raw, std::align_val_t{64});

    return 0;
}
