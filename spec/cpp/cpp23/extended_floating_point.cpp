// Extended floating-point types — C++23
// What: standard typedefs for fixed-width FP: std::float16_t, float32_t,
//       float64_t, float128_t, bfloat16_t.
// Why:  ML and graphics code needs explicit half precision and bfloat16; these
//       used to be vendor extensions.
// Pre:  __fp16, _Float16, vendor-specific intrinsics, or third-party headers.
//
// libc++ feature test: __cpp_lib_extended_float (Apple clang 21: NOT defined)

#include <iostream>
#include <version>

#if defined(__cpp_lib_extended_float) && __cpp_lib_extended_float >= 202311L
#  include <stdfloat>
#  define HAVE_EXTFLOAT 1
#else
#  define HAVE_EXTFLOAT 0
#endif

int main() {
#if HAVE_EXTFLOAT
    std::float16_t  half  = 1.5f;
    std::float32_t  single = 3.14f;
    std::float64_t  dbl    = 2.71828;
    std::bfloat16_t bf     = 1.0f;
    std::cout << "half=" << static_cast<float>(half)
              << " single=" << single
              << " dbl=" << dbl
              << " bf=" << static_cast<float>(bf) << "\n";
#else
    std::cout << "<stdfloat> extended FP types not available in this libc++.\n";
    std::cout << "Workaround: vendor extensions (e.g. _Float16) or library headers.\n";
    float dummy = 1.5f;
    std::cout << "regular float: " << dummy << "\n";
#endif
    return 0;
}
