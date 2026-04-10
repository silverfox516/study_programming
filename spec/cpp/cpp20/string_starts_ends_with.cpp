// std::string::starts_with / ends_with — C++20
// What: simple prefix/suffix membership predicates on strings and string_views.
// Why:  Python-style API for the most common substring check; clearer than
//       `s.compare(0, n, prefix) == 0` or `s.find(prefix) == 0`.
// Pre:  ad-hoc helpers in every codebase, often subtly wrong on empty strings.

#include <iostream>
#include <string>
#include <string_view>

int main() {
    std::string path = "/usr/local/bin/clang";

    std::cout << path << "\n";
    std::cout << "  starts_with('/usr'):  " << path.starts_with("/usr") << "\n";
    std::cout << "  starts_with('/etc'):  " << path.starts_with("/etc") << "\n";
    std::cout << "  ends_with('clang'):   " << path.ends_with("clang")  << "\n";
    std::cout << "  ends_with('.exe'):    " << path.ends_with(".exe")   << "\n";

    // Both also accept a single char
    std::cout << "  starts_with('/'):     " << path.starts_with('/')    << "\n";

    // string_view has the same members — works on non-owning slices
    std::string_view sv("config.toml");
    std::cout << sv << " ends_with .toml: " << sv.ends_with(".toml") << "\n";

    // Note: std::string::contains is C++23, not C++20.

    return 0;
}
