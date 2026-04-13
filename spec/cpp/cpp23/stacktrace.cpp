// C++23 <stacktrace> — programmatic stack traces
// Note: not yet supported by Apple Clang / libc++

#include <iostream>

#if __has_include(<stacktrace>)
#include <stacktrace>
#define HAS_STACKTRACE 1
#else
#define HAS_STACKTRACE 0
#endif

void g() {
#if HAS_STACKTRACE && defined(__cpp_lib_stacktrace)
    auto st = std::stacktrace::current();
    std::cout << "Stack trace (" << st.size() << " frames):\n" << st << "\n";
#else
    std::cout << "[<stacktrace> not available on this toolchain]\n";
    std::cout << "  std::stacktrace::current() would capture the call stack\n";
    std::cout << "  Each entry has: description(), source_file(), source_line()\n";
#endif
}

void f() { g(); }

int main() {
    std::cout << "=== C++23 stacktrace ===\n";
    f();
    return 0;
}
