// Nested namespace definitions — C++17
// What: namespace A::B::C { ... } as a single-line shortcut for three nested namespaces.
// Why:  removes triple-indented namespace nesting that added pure noise.
// Pre:  three opening braces, three closing braces, three indent levels.

#include <iostream>

// Old way:
// namespace company {
//     namespace project {
//         namespace utils {
//             int answer() { return 42; }
//         }
//     }
// }

// C++17 way:
namespace company::project::utils {
    int answer() { return 42; }
    const char* name() { return "study"; }
}

int main() {
    std::cout << company::project::utils::name() << " says "
              << company::project::utils::answer() << "\n";
    return 0;
}
