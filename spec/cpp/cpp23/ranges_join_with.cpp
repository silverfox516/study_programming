// std::views::join_with — C++23
// What: flattens a range of ranges, inserting a fixed delimiter between each
//       inner range. Like Python's `delim.join(parts)` for ranges.
// Why:  the most common "join with separator" idiom finally has a standard form.
// Pre:  loops with a "first" flag to suppress the leading separator.
//
// libc++ feature test: __cpp_lib_ranges_join_with (Apple clang 21 reports NOT defined)

#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <version>

#if defined(__cpp_lib_ranges_join_with) && __cpp_lib_ranges_join_with >= 202202L
#  include <ranges>
#  define HAVE_JW 1
#else
#  define HAVE_JW 0
#endif

int main() {
    std::vector<std::string> parts{"foo", "bar", "baz"};

#if HAVE_JW
    std::string out;
    for (char c : parts | std::views::join_with(std::string_view(", "))) {
        out += c;
    }
    std::cout << out << "\n";
#else
    std::cout << "<ranges> std::views::join_with not available in this libc++.\n";
    std::string out;
    bool first = true;
    for (const auto& p : parts) {
        if (!first) out += ", ";
        first = false;
        out += p;
    }
    std::cout << out << "\n";
#endif
    return 0;
}
