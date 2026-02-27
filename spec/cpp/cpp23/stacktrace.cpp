#include <iostream>
#include <stacktrace>

void g() {
    std::cout << std::stacktrace::current() << std::endl;
}

void f() {
    g();
}

int main() {
    f();
    return 0;
}
