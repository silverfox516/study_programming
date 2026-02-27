#include <iostream>

struct Base {
    virtual void foo() {
        std::cout << "Base::foo" << std::endl;
    }
    virtual void bar() {
        std::cout << "Base::bar" << std::endl;
    }
};

struct Derived : Base {
    void foo() override { // Correctly overrides Base::foo
        std::cout << "Derived::foo" << std::endl;
    }
    // void baz() override; // Error: Base::baz is not virtual

    void bar() final { // Prevents further overriding
        std::cout << "Derived::bar" << std::endl;
    }
};

struct Grandchild : Derived {
    // void bar() override; // Error: Derived::bar is final
};


int main() {
    Derived d;
    d.foo();
    d.bar();
    return 0;
}
