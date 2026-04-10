// std::print / std::println — C++23
// What: thin wrappers around std::format that write directly to stdout (or a stream).
//       println adds a trailing newline.
// Why:  std::cout chains with `<<` and global state (locale, ios flags) make it
//       slow and easy to misuse. print/println are formatted, type-safe, and fast.
// Pre:  printf (no type safety, locale-bound) or std::cout (verbose, slower).
// See also: spec/cpp/cpp23/print.cpp may already cover the basics.

#include <print>
#include <string>

int main() {
    std::println("hello, {}!", "world");
    std::println("{} + {} = {}", 2, 3, 2 + 3);

    // print without newline
    std::print("count down: ");
    for (int i = 5; i > 0; --i) std::print("{} ", i);
    std::print("go!\n");

    // Width and formatting via std::format mini-language
    for (const std::string& name : {"Alice", "Bob", "Carol"}) {
        std::println("[{:>10}]", name);
    }

    // Unlike printf, type errors are caught at compile time
    // std::println("{}", 1, 2, 3);  // would fail to compile — extra args

    return 0;
}
