// std::quoted — C++14
// What: a stream manipulator that wraps a string in quotes when writing, and
//       reads back a quoted token (handling escaped quotes inside) when reading.
// Why:  preserves strings with embedded whitespace through round-trip stream I/O,
//       which the default operator>> cannot do (it stops at the first space).
// Pre:  manual escaping/unescaping or hand-rolled tokenizers.

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

int main() {
    std::string original = "hello world with \"quotes\" inside";
    std::cout << "original: " << original << "\n";

    // Write with std::quoted
    std::stringstream ss;
    ss << std::quoted(original);
    std::cout << "encoded:  " << ss.str() << "\n";

    // Read back
    std::string round_trip;
    ss >> std::quoted(round_trip);
    std::cout << "decoded:  " << round_trip << "\n";

    std::cout << "match:    " << (original == round_trip ? "yes" : "no") << "\n";

    return 0;
}
