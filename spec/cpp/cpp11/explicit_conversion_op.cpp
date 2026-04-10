// explicit conversion operators — C++11
// What: a member operator T() can be marked `explicit`, blocking implicit conversion.
// Why:  the classic "safe bool" idiom — `if (smart_ptr)` should work, but
//       `int x = smart_ptr;` should not.
// Pre:  the "safe bool idiom" needed a private nested type pointer trick.

#include <iostream>

class Handle {
public:
    explicit Handle(int id) : id_(id) {}

    // Allowed in `if (h)` and `static_cast<bool>(h)`,
    // but not in `bool b = h;` or `int n = h;`.
    explicit operator bool() const { return id_ != 0; }

private:
    int id_;
};

int main() {
    Handle good(42);
    Handle bad(0);

    if (good) std::cout << "good is truthy\n";
    if (!bad) std::cout << "bad is falsy\n";

    // bool b = good;          // would fail to compile — needs explicit cast
    bool b = static_cast<bool>(good);
    std::cout << "explicit cast: " << b << "\n";

    // int n = good;           // would also fail — no implicit chain to int
    return 0;
}
