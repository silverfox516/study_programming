// __has_include — C++17 (also widely supported as a vendor extension before)
// What: a preprocessor predicate that is true if the named header is available.
// Why:  lets a header degrade gracefully across compilers and library versions.
// Pre:  feature detection via configure scripts (autoconf), or assumptions baked into ifdefs.

#include <iostream>

#if __has_include(<optional>)
#  include <optional>
#  define HAVE_OPTIONAL 1
#else
#  define HAVE_OPTIONAL 0
#endif

#if __has_include(<this_header_does_not_exist>)
#  define HAVE_FAKE 1
#else
#  define HAVE_FAKE 0
#endif

int main() {
    std::cout << "have <optional>:                  " << HAVE_OPTIONAL << "\n";
    std::cout << "have <this_header_does_not_exist>:" << HAVE_FAKE     << "\n";

#if HAVE_OPTIONAL
    std::optional<int> v = 42;
    std::cout << "optional value: " << v.value() << "\n";
#endif

    return 0;
}
