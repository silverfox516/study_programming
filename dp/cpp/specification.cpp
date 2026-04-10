// Specification — DDD Pattern
//
// Intent: encapsulate a business rule (a predicate) as an object so it can be
//         combined with AND/OR/NOT, passed around, persisted, or translated
//         into a database query.
//
// When to use:
//   - Business rules are non-trivial and reused across many places
//   - You want to combine rules dynamically (e.g. user-built filters)
//   - You may want to render the same rule both in code and in SQL
//
// When NOT to use:
//   - The rule is one line in one place — just write it inline
//   - Performance-critical hot path — virtual dispatch overhead matters

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <utility>
#include <functional>

// ============================================================================
// Spec interface and combinators
// ============================================================================
template <typename T>
class Specification {
public:
    virtual ~Specification() = default;
    virtual bool is_satisfied_by(const T& candidate) const = 0;
};

template <typename T>
using SpecPtr = std::shared_ptr<Specification<T>>;

// AND combinator
template <typename T>
class AndSpec : public Specification<T> {
public:
    AndSpec(SpecPtr<T> a, SpecPtr<T> b) : a_(std::move(a)), b_(std::move(b)) {}
    bool is_satisfied_by(const T& c) const override {
        return a_->is_satisfied_by(c) && b_->is_satisfied_by(c);
    }
private:
    SpecPtr<T> a_;
    SpecPtr<T> b_;
};

// OR combinator
template <typename T>
class OrSpec : public Specification<T> {
public:
    OrSpec(SpecPtr<T> a, SpecPtr<T> b) : a_(std::move(a)), b_(std::move(b)) {}
    bool is_satisfied_by(const T& c) const override {
        return a_->is_satisfied_by(c) || b_->is_satisfied_by(c);
    }
private:
    SpecPtr<T> a_;
    SpecPtr<T> b_;
};

// NOT combinator
template <typename T>
class NotSpec : public Specification<T> {
public:
    explicit NotSpec(SpecPtr<T> s) : s_(std::move(s)) {}
    bool is_satisfied_by(const T& c) const override { return !s_->is_satisfied_by(c); }
private:
    SpecPtr<T> s_;
};

// Named combinators (operators && || ! on shared_ptr conflict with built-in bool)
template <typename T>
SpecPtr<T> all_of(SpecPtr<T> a, SpecPtr<T> b) {
    return std::make_shared<AndSpec<T>>(std::move(a), std::move(b));
}
template <typename T>
SpecPtr<T> any_of(SpecPtr<T> a, SpecPtr<T> b) {
    return std::make_shared<OrSpec<T>>(std::move(a), std::move(b));
}
template <typename T>
SpecPtr<T> negate(SpecPtr<T> s) {
    return std::make_shared<NotSpec<T>>(std::move(s));
}

// ============================================================================
// Domain
// ============================================================================
struct Product {
    std::string  name;
    std::string  category;
    double       price;
    int          stock;
};

// Concrete specifications
class InCategorySpec : public Specification<Product> {
public:
    explicit InCategorySpec(std::string cat) : cat_(std::move(cat)) {}
    bool is_satisfied_by(const Product& p) const override { return p.category == cat_; }
private:
    std::string cat_;
};

class PriceBelowSpec : public Specification<Product> {
public:
    explicit PriceBelowSpec(double max) : max_(max) {}
    bool is_satisfied_by(const Product& p) const override { return p.price < max_; }
private:
    double max_;
};

class InStockSpec : public Specification<Product> {
public:
    bool is_satisfied_by(const Product& p) const override { return p.stock > 0; }
};

// ============================================================================
// Demo
// ============================================================================
int main() {
    std::cout << "=== Specification Pattern ===\n";

    std::vector<Product> products = {
        {"Laptop",   "electronics", 999.99,  5},
        {"Phone",    "electronics", 599.99,  0},
        {"Book",     "books",        15.00, 50},
        {"Headset",  "electronics",  79.99, 20},
        {"Notebook", "stationery",    3.50,  0},
    };

    // Build a complex rule by combining simpler specs
    SpecPtr<Product> cheap_electronics = all_of<Product>(
        all_of<Product>(
            std::make_shared<InCategorySpec>("electronics"),
            std::make_shared<PriceBelowSpec>(100.0)),
        std::make_shared<InStockSpec>());

    std::cout << "matching 'cheap electronics in stock':\n";
    for (const auto& p : products) {
        if (cheap_electronics->is_satisfied_by(p)) {
            std::cout << "  - " << p.name << " ($" << p.price << ", stock " << p.stock << ")\n";
        }
    }

    // Negate a spec
    SpecPtr<Product> out_of_stock = negate<Product>(std::make_shared<InStockSpec>());
    std::cout << "out of stock:\n";
    for (const auto& p : products) {
        if (out_of_stock->is_satisfied_by(p)) {
            std::cout << "  - " << p.name << "\n";
        }
    }

    std::cout << "\nKey points:\n";
    std::cout << " * Each rule is a small object with one method\n";
    std::cout << " * AND/OR/NOT combinators give a rule algebra\n";
    std::cout << " * Same rule can be evaluated in code or translated to SQL\n";
    std::cout << " * Easier to test and reuse than free predicate functions in many cases\n";
    return 0;
}
