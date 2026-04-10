// Feature test macros — C++20 (formalized; some predate C++20 as conventions)
// What: predefined macros like __cpp_lib_format that tell you whether the current
//       library implementation supports a given feature.
// Why:  toolchains roll out C++20/23 features incrementally; portable code needs to
//       detect what's actually available rather than assume.
// Pre:  hand-rolled compiler/version checks via __GNUC__ etc.

#include <iostream>
#include <version>      // brings in the __cpp_lib_* feature test macros

template <typename T>
void check(const char* name, T value) {
    std::cout << name << ": " << value << "\n";
}

int main() {
#ifdef __cpp_concepts
    check("__cpp_concepts",     __cpp_concepts);
#else
    std::cout << "__cpp_concepts not defined\n";
#endif

#ifdef __cpp_lib_format
    check("__cpp_lib_format",   __cpp_lib_format);
#else
    std::cout << "__cpp_lib_format not defined\n";
#endif

#ifdef __cpp_lib_ranges
    check("__cpp_lib_ranges",   __cpp_lib_ranges);
#else
    std::cout << "__cpp_lib_ranges not defined\n";
#endif

#ifdef __cpp_lib_jthread
    check("__cpp_lib_jthread",  __cpp_lib_jthread);
#else
    std::cout << "__cpp_lib_jthread not defined\n";
#endif

#ifdef __cpp_lib_atomic_ref
    check("__cpp_lib_atomic_ref", __cpp_lib_atomic_ref);
#else
    std::cout << "__cpp_lib_atomic_ref not defined\n";
#endif

    // The macro values are the year+month the feature was voted in; you compare
    // them numerically to require a minimum version.
#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
    std::cout << "format is at least the original 2019-07 version\n";
#endif

    return 0;
}
