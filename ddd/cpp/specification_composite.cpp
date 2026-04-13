// Specification Pattern (DDD Composite)
// Encapsulate business rules as composable, reusable objects.
// Supports AND, OR, NOT combinations for complex queries.

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

// ============================================================
// Domain entity
// ============================================================
struct Product {
    std::string name;
    double price;
    std::string category;
    bool in_stock;
    double rating; // 0.0 - 5.0
};

// ============================================================
// Specification base
// ============================================================
class Specification {
public:
    virtual ~Specification() = default;
    virtual bool is_satisfied_by(const Product& p) const = 0;

    // Fluent composition
    std::unique_ptr<Specification> and_spec(std::unique_ptr<Specification> other) const;
    std::unique_ptr<Specification> or_spec(std::unique_ptr<Specification> other) const;
    std::unique_ptr<Specification> not_spec() const;
};

// ============================================================
// Composite specifications
// ============================================================
class AndSpec : public Specification {
    std::unique_ptr<Specification> a_, b_;
public:
    AndSpec(std::unique_ptr<Specification> a, std::unique_ptr<Specification> b)
        : a_(std::move(a)), b_(std::move(b)) {}
    bool is_satisfied_by(const Product& p) const override {
        return a_->is_satisfied_by(p) && b_->is_satisfied_by(p);
    }
};

class OrSpec : public Specification {
    std::unique_ptr<Specification> a_, b_;
public:
    OrSpec(std::unique_ptr<Specification> a, std::unique_ptr<Specification> b)
        : a_(std::move(a)), b_(std::move(b)) {}
    bool is_satisfied_by(const Product& p) const override {
        return a_->is_satisfied_by(p) || b_->is_satisfied_by(p);
    }
};

class NotSpec : public Specification {
    std::unique_ptr<Specification> inner_;
public:
    explicit NotSpec(std::unique_ptr<Specification> inner) : inner_(std::move(inner)) {}
    bool is_satisfied_by(const Product& p) const override {
        return !inner_->is_satisfied_by(p);
    }
};

// Fluent methods (defined after composite classes)
// Note: these clone the current spec conceptually — simplified for demo
std::unique_ptr<Specification> Specification::and_spec(std::unique_ptr<Specification> other) const {
    // For chaining, caller manages ownership externally
    return nullptr; // placeholder — real impl would clone
}
std::unique_ptr<Specification> Specification::or_spec(std::unique_ptr<Specification> other) const {
    return nullptr;
}
std::unique_ptr<Specification> Specification::not_spec() const {
    return nullptr;
}

// ============================================================
// Concrete specifications (business rules)
// ============================================================
class PriceBelow : public Specification {
    double max_;
public:
    explicit PriceBelow(double max) : max_(max) {}
    bool is_satisfied_by(const Product& p) const override { return p.price < max_; }
};

class InCategory : public Specification {
    std::string category_;
public:
    explicit InCategory(std::string cat) : category_(std::move(cat)) {}
    bool is_satisfied_by(const Product& p) const override { return p.category == category_; }
};

class InStock : public Specification {
public:
    bool is_satisfied_by(const Product& p) const override { return p.in_stock; }
};

class HighRated : public Specification {
    double min_rating_;
public:
    explicit HighRated(double min) : min_rating_(min) {}
    bool is_satisfied_by(const Product& p) const override { return p.rating >= min_rating_; }
};

// ============================================================
// Repository query using specifications
// ============================================================
std::vector<const Product*> query(
    const std::vector<Product>& products,
    const Specification& spec)
{
    std::vector<const Product*> result;
    for (const auto& p : products)
        if (spec.is_satisfied_by(p))
            result.push_back(&p);
    return result;
}

void print_results(const std::string& label, const std::vector<const Product*>& results) {
    std::cout << label << " (" << results.size() << " results):\n";
    for (const auto* p : results)
        std::cout << "  " << p->name << " ($" << p->price
                  << ", " << p->category << ", rating=" << p->rating
                  << ", " << (p->in_stock ? "in stock" : "out of stock") << ")\n";
}

int main() {
    std::vector<Product> catalog = {
        {"Laptop", 999.99, "electronics", true, 4.5},
        {"Mouse", 29.99, "electronics", true, 4.2},
        {"Keyboard", 79.99, "electronics", false, 3.8},
        {"Novel", 14.99, "books", true, 4.7},
        {"Textbook", 89.99, "books", true, 3.5},
        {"Headphones", 199.99, "electronics", true, 4.8},
    };

    // Simple specs
    PriceBelow cheap(50.0);
    InCategory electronics("electronics");
    InStock in_stock;
    HighRated well_rated(4.0);

    print_results("Cheap (<$50)", query(catalog, cheap));

    // Composite: cheap electronics
    AndSpec cheap_electronics(
        std::make_unique<PriceBelow>(100.0),
        std::make_unique<InCategory>("electronics")
    );
    print_results("\nCheap electronics (<$100)", query(catalog, cheap_electronics));

    // Composite: in-stock AND high-rated
    AndSpec available_and_good(
        std::make_unique<InStock>(),
        std::make_unique<HighRated>(4.0)
    );
    print_results("\nIn-stock & high-rated (>=4.0)", query(catalog, available_and_good));

    // Composite: NOT electronics
    NotSpec non_electronics(std::make_unique<InCategory>("electronics"));
    print_results("\nNon-electronics", query(catalog, non_electronics));

    // Complex: (cheap OR high-rated) AND in-stock
    AndSpec deal_finder(
        std::make_unique<OrSpec>(
            std::make_unique<PriceBelow>(50.0),
            std::make_unique<HighRated>(4.5)
        ),
        std::make_unique<InStock>()
    );
    print_results("\nDeals (cheap OR top-rated) & in-stock", query(catalog, deal_finder));

    return 0;
}
