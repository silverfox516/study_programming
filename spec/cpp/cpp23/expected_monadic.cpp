// std::expected monadic operations — C++23
// What: and_then, transform, or_else, transform_error chain expected<T, E> values
//       without bare if/else error checking.
// Why:  the same monadic shape that std::optional got in C++23, applied to
//       expected. Composes error-handling pipelines.
// Pre:  manual `if (!result) return result.error();` after every step.
// See also: spec/cpp/cpp23/expected.cpp covers basic construction and access.

#include <iostream>
#include <expected>
#include <string>

std::expected<int, std::string> parse(const std::string& s) {
    try {
        return std::stoi(s);
    } catch (...) {
        return std::unexpected("not a number: " + s);
    }
}

std::expected<int, std::string> nonzero(int n) {
    if (n == 0) return std::unexpected("must be non-zero");
    return n;
}

int main() {
    auto pipeline = [](const std::string& s) {
        return parse(s)
             .and_then(nonzero)
             .transform([](int n) { return 100 / n; })
             .transform_error([](const std::string& e) { return "error: " + e; });
    };

    for (const std::string& input : {"5", "0", "abc"}) {
        auto r = pipeline(input);
        if (r) std::cout << input << " -> " << *r << "\n";
        else   std::cout << input << " -> [" << r.error() << "]\n";
    }

    // or_else lets you recover with a fallback expected
    auto fallback = parse("oops").or_else([](const std::string&) -> std::expected<int, std::string> {
        return 0;
    });
    std::cout << "fallback: " << *fallback << "\n";

    return 0;
}
