// CRTP — Curiously Recurring Template Pattern — C++ Idiom
//
// Intent: a base class template takes the derived class as a template
//         parameter, enabling static polymorphism: the base can call
//         derived methods without virtual dispatch.
//
// When to use:
//   - You want polymorphic behavior at zero runtime cost (no vtable)
//   - Mixins that inject behavior (operator overloads, serialization, cloning)
//   - Static interfaces checked at compile time
//
// When NOT to use:
//   - You need runtime polymorphism (heterogeneous containers, plugins)
//   - The derived set is open-ended — template bloat grows with each type

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// ============================================================================
// Example 1: Static polymorphism — no virtual dispatch
// ============================================================================
template <typename Derived>
class Shape {
public:
    double area() const {
        return static_cast<const Derived*>(this)->area_impl();
    }
    std::string name() const {
        return static_cast<const Derived*>(this)->name_impl();
    }
    void describe() const {
        std::cout << "  " << name() << " area=" << area() << "\n";
    }
};

class Circle : public Shape<Circle> {
public:
    explicit Circle(double r) : r_(r) {}
    double      area_impl() const { return 3.14159 * r_ * r_; }
    std::string name_impl() const { return "Circle(r=" + std::to_string(r_) + ")"; }
private:
    double r_;
};

class Square : public Shape<Square> {
public:
    explicit Square(double s) : s_(s) {}
    double      area_impl() const { return s_ * s_; }
    std::string name_impl() const { return "Square(s=" + std::to_string(s_) + ")"; }
private:
    double s_;
};

// ============================================================================
// Example 2: Mixin — inject comparison operators from a single `less_than`
// ============================================================================
template <typename Derived>
class Comparable {
public:
    friend bool operator>(const Derived& a, const Derived& b)  { return b < a; }
    friend bool operator<=(const Derived& a, const Derived& b) { return !(b < a); }
    friend bool operator>=(const Derived& a, const Derived& b) { return !(a < b); }
};

class Temperature : public Comparable<Temperature> {
public:
    explicit Temperature(double c) : celsius_(c) {}
    friend bool operator<(const Temperature& a, const Temperature& b) {
        return a.celsius_ < b.celsius_;
    }
    double value() const { return celsius_; }
private:
    double celsius_;
};

// ============================================================================
// Example 3: Static clone
// ============================================================================
template <typename Derived>
class Cloneable {
public:
    std::unique_ptr<Derived> clone() const {
        return std::make_unique<Derived>(static_cast<const Derived&>(*this));
    }
};

class Config : public Cloneable<Config> {
public:
    Config(std::string k, int v) : key(std::move(k)), value(v) {}
    std::string key;
    int         value;
};

int main() {
    std::cout << "=== CRTP Pattern ===\n";

    Circle c(5);
    Square s(4);
    c.describe();
    s.describe();

    Temperature t1(20), t2(30);
    std::cout << "20 < 30: " << (t1 < t2)  << "\n";
    std::cout << "20 > 30: " << (t1 > t2)  << "\n";
    std::cout << "20 >= 20:" << (t1 >= t1) << "\n";

    Config cfg("mode", 1);
    auto copy = cfg.clone();
    std::cout << "cloned: " << copy->key << "=" << copy->value << "\n";

    std::cout << "\nKey points:\n";
    std::cout << " * Base calls Derived methods via static_cast — no virtual\n";
    std::cout << " * Each instantiation is a different type — no heterogeneous container\n";
    std::cout << " * Great for mixins: inject operators, clone, serialization\n";
    std::cout << " * C++23 'deducing this' can replace many CRTP uses\n";
    return 0;
}
