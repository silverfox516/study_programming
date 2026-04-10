// std::string::contains / string_view::contains — C++23
// What: a substring/character predicate that completes the trio with starts_with
//       and ends_with from C++20.
// Why:  the obvious member that was missing in C++20. find() returning a position
//       comparable to npos was a frequent source of off-by-one bugs.
// Pre:  `s.find(needle) != std::string::npos` — easy to type wrong.

#include <iostream>
#include <string>
#include <string_view>

int main() {
    std::string sentence = "the quick brown fox jumps over the lazy dog";

    std::cout << std::boolalpha;
    std::cout << "contains 'fox':   " << sentence.contains("fox")  << "\n";
    std::cout << "contains 'cat':   " << sentence.contains("cat")  << "\n";
    std::cout << "contains 'q':     " << sentence.contains('q')    << "\n";
    std::cout << "contains '':      " << sentence.contains("")     << "\n";  // true: empty is substring of everything

    std::string_view sv("config.toml.bak");
    std::cout << "view contains '.toml': " << sv.contains(".toml") << "\n";

    return 0;
}
