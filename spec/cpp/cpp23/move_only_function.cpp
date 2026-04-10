// std::move_only_function — C++23
// What: a type-erased callable wrapper like std::function, but only requires
//       (and only supports) MoveConstructible callables — not Copyable.
// Why:  std::function rejects move-only lambdas (e.g. captured unique_ptr), forcing
//       awkward shared_ptr workarounds. move_only_function fixes this.
// Pre:  std::function, plus pain whenever a lambda captured a unique_ptr.
//
// libc++ feature test: __cpp_lib_move_only_function (Apple clang 21 reports NOT defined)

#include <iostream>
#include <memory>
#include <functional>
#include <version>

#if defined(__cpp_lib_move_only_function) && __cpp_lib_move_only_function >= 202110L
#  define HAVE_MOF 1
#else
#  define HAVE_MOF 0
#endif

int main() {
#if HAVE_MOF
    // A lambda capturing a unique_ptr — std::function would reject this
    auto up = std::make_unique<int>(42);
    std::move_only_function<int()> f = [up = std::move(up)]() {
        return *up;
    };

    std::cout << "f() = " << f() << "\n";

    // f is itself move-only — copy is deleted
    auto g = std::move(f);
    std::cout << "g() = " << g() << "\n";
#else
    std::cout << "<functional> std::move_only_function not available in this libc++.\n";
    std::cout << "Workaround: wrap the move-only state in a std::shared_ptr to use std::function.\n";

    // Demonstrate the workaround
    auto up = std::make_shared<int>(42);
    std::function<int()> f = [up]() { return *up; };
    std::cout << "shared_ptr workaround: f() = " << f() << "\n";
#endif
    return 0;
}
