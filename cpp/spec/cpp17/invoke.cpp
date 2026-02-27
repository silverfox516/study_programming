#include <iostream>
#include <functional>

void print_num(int i) {
    std::cout << "Function: " << i << std::endl;
}

struct Foo {
    void print_num(int i) const {
        std::cout << "Member function: " << i << std::endl;
    }
    int num = 10;
};

int main() {
    // Invoke function
    std::invoke(print_num, 10);

    // Invoke member function
    Foo foo;
    std::invoke(&Foo::print_num, foo, 20);

    // Invoke member variable (access)
    std::cout << "Member variable: " << std::invoke(&Foo::num, foo) << std::endl;

    // Invoke lambda
    std::invoke([]() { std::cout << "Lambda invoked" << std::endl; });

    return 0;
}
