// std::boyer_moore_searcher / boyer_moore_horspool_searcher — C++17
// What: function objects passed to std::search that implement faster substring search
//       algorithms than the default O(n*m) scan.
// Why:  for long haystacks and non-trivial needles, Boyer-Moore preprocesses the needle
//       once and then skips through the haystack in big jumps.
// Pre:  hand-rolled implementations or external libraries.

#include <iostream>
#include <algorithm>
#include <functional>
#include <string>

int main() {
    std::string haystack =
        "the quick brown fox jumps over the lazy dog and then the fox sleeps";
    std::string needle = "fox";

    // Default search — works but is the simple O(n*m) algorithm
    auto it1 = std::search(haystack.begin(), haystack.end(),
                           needle.begin(), needle.end());
    if (it1 != haystack.end()) {
        std::cout << "default search found at offset "
                  << (it1 - haystack.begin()) << "\n";
    }

    // Boyer-Moore — preprocesses the needle once, then skips fast
    auto it2 = std::search(haystack.begin(), haystack.end(),
                           std::boyer_moore_searcher(needle.begin(), needle.end()));
    if (it2 != haystack.end()) {
        std::cout << "boyer_moore found at offset "
                  << (it2 - haystack.begin()) << "\n";
    }

    // Boyer-Moore-Horspool — simpler variant, less preprocessing
    auto it3 = std::search(haystack.begin(), haystack.end(),
                           std::boyer_moore_horspool_searcher(needle.begin(), needle.end()));
    if (it3 != haystack.end()) {
        std::cout << "horspool found at offset "
                  << (it3 - haystack.begin()) << "\n";
    }

    return 0;
}
