// Value Object — DDD Building Block
//
// Intent: an object defined entirely by its attributes, not by an identity.
//         Two value objects with the same data ARE the same thing. Immutable
//         by convention — you create a new one rather than mutating.
//
// When to use:
//   - Money, coordinates, color, date ranges, temperature
//   - Equality is by content, not by pointer/ID
//   - You want safe sharing and hashing without worrying about mutation
//
// When NOT to use:
//   - The object has a lifecycle and needs tracking (that's an Entity)

#include <iostream>
#include <string>
#include <cmath>
#include <unordered_set>
#include <functional>

// ============================================================================
// Example 1: Money
// ============================================================================
class Money {
public:
    Money(double amount, std::string currency)
        : amount_(amount), currency_(std::move(currency)) {}

    double             amount()   const { return amount_; }
    const std::string& currency() const { return currency_; }

    Money add(const Money& other) const {
        if (currency_ != other.currency_)
            throw std::runtime_error("currency mismatch");
        return Money(amount_ + other.amount_, currency_);
    }

    bool operator==(const Money& o) const {
        return currency_ == o.currency_ && std::abs(amount_ - o.amount_) < 1e-9;
    }
    bool operator!=(const Money& o) const { return !(*this == o); }

private:
    double      amount_;
    std::string currency_;
};

// ============================================================================
// Example 2: Coordinate — naturally hashable
// ============================================================================
struct Coord {
    int x;
    int y;
    bool operator==(const Coord& o) const { return x == o.x && y == o.y; }
};

struct CoordHash {
    std::size_t operator()(const Coord& c) const {
        auto h = std::hash<int>{};
        return h(c.x) ^ (h(c.y) << 16);
    }
};

// ============================================================================
// Demo
// ============================================================================
int main() {
    std::cout << "=== Value Object Pattern ===\n";

    Money a(10.0, "USD");
    Money b(10.0, "USD");
    Money c( 5.0, "USD");
    std::cout << "a == b: " << (a == b) << "\n";  // true — same value
    std::cout << "a == c: " << (a == c) << "\n";  // false

    Money total = a.add(c);
    std::cout << "total: " << total.amount() << " " << total.currency() << "\n";

    // Value objects are perfect as unordered_set keys
    std::unordered_set<Coord, CoordHash> visited;
    visited.insert({0, 0});
    visited.insert({1, 0});
    visited.insert({0, 0});   // duplicate — set ignores it
    std::cout << "visited set size: " << visited.size() << "\n";

    std::cout << "\nKey points:\n";
    std::cout << " * Equality is by content, not by identity/pointer\n";
    std::cout << " * Immutability (or copy-on-write) prevents aliasing bugs\n";
    std::cout << " * Safe to use as map/set keys\n";
    std::cout << " * Two instances with the same data are interchangeable\n";
    return 0;
}
