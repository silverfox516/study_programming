// Delegating and inheriting constructors — C++11
// What: a constructor can call a sibling constructor in its initializer list (delegation),
//       and a derived class can pull base constructors in with `using Base::Base;` (inheriting).
// Why:  removes copy-pasted constructor bodies and saves writing forwarding ctors by hand.
// Pre:  initializer logic was duplicated across constructors, or moved into a private init() helper.

#include <iostream>
#include <string>

// --- Delegating constructors ---
class Point {
public:
    // Target constructor — does the real work
    Point(int x, int y, std::string label)
        : x_(x), y_(y), label_(std::move(label)) {
        std::cout << "Point(" << x_ << "," << y_ << ",\"" << label_ << "\")\n";
    }

    // Delegate to the target with a default label
    Point(int x, int y) : Point(x, y, "unnamed") {}

    // Delegate to the previous one with a default position
    Point() : Point(0, 0) {}

private:
    int         x_;
    int         y_;
    std::string label_;
};

// --- Inheriting constructors ---
struct Base {
    Base(int n)               { std::cout << "Base(int " << n << ")\n"; }
    Base(const std::string& s){ std::cout << "Base(string " << s << ")\n"; }
};

struct Derived : Base {
    using Base::Base;  // pull all Base constructors into Derived
};

int main() {
    Point p1;
    Point p2(1, 2);
    Point p3(5, 6, "origin-ish");

    Derived d1(42);
    Derived d2(std::string("hi"));

    return 0;
}
