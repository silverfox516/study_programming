// std::launder — C++17
// What: tells the compiler "I know I just placement-newed a different object in this
//       storage, please don't reuse cached assumptions about the previous object."
// Why:  the optimizer can otherwise prove things about the original object's lifetime
//       and miscompile code that placement-news a replacement of a different dynamic type.
// Pre:  no portable solution; you had to hope the compiler did the right thing.
// Note: this is an expert-only tool. Most code never needs it.

#include <iostream>
#include <new>

struct Base {
    virtual int answer() const { return 1; }
    virtual ~Base() = default;
};

struct Derived : Base {
    int answer() const override { return 42; }
};

int main() {
    alignas(Derived) unsigned char storage[sizeof(Derived)];

    // Construct a Base in storage
    Base* b = new (storage) Base;
    std::cout << "first: " << b->answer() << "\n";
    b->~Base();

    // Construct a Derived in the same storage. The original pointer `b` may
    // not be safely reused even if you cast — the compiler is allowed to
    // believe `b` still points to a Base. std::launder gets you a fresh pointer
    // the optimizer treats as having no history.
    Derived* d = new (storage) Derived;
    Base*    b2 = std::launder(reinterpret_cast<Base*>(storage));
    std::cout << "via launder: " << b2->answer() << "\n";
    d->~Derived();

    return 0;
}
