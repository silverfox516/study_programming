// Clean Architecture
// Concentric layers: Entities → Use Cases → Interface Adapters → Frameworks
// Dependencies point inward only.

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <stdexcept>

// ============================================================
// Layer 1 (innermost): ENTITIES — enterprise business rules
// ============================================================
namespace entities {

class Product {
    std::string name_;
    double price_;
    int stock_;
public:
    Product(std::string name, double price, int stock)
        : name_(std::move(name)), price_(price), stock_(stock) {}

    const std::string& name() const { return name_; }
    double price() const { return price_; }
    int stock() const { return stock_; }

    void reduce_stock(int qty) {
        if (qty > stock_) throw std::runtime_error("Not enough stock for " + name_);
        stock_ -= qty;
    }
};

} // namespace entities

// ============================================================
// Layer 2: USE CASES — application business rules
// ============================================================
namespace usecases {

// Port: output boundary
class ProductRepository {
public:
    virtual ~ProductRepository() = default;
    virtual entities::Product* find(const std::string& name) = 0;
    virtual void save(const entities::Product& p) = 0;
};

class Logger {
public:
    virtual ~Logger() = default;
    virtual void log(const std::string& msg) = 0;
};

// Use case: Purchase Product
struct PurchaseRequest {
    std::string product_name;
    int quantity;
};

struct PurchaseResponse {
    bool success;
    std::string message;
    double total;
};

class PurchaseProduct {
    ProductRepository& repo_;
    Logger& logger_;
public:
    PurchaseProduct(ProductRepository& repo, Logger& logger)
        : repo_(repo), logger_(logger) {}

    PurchaseResponse execute(const PurchaseRequest& req) {
        auto* product = repo_.find(req.product_name);
        if (!product)
            return {false, "Product not found", 0};

        try {
            product->reduce_stock(req.quantity);
            double total = product->price() * req.quantity;
            repo_.save(*product);
            logger_.log("Purchased " + std::to_string(req.quantity) + "x " + req.product_name);
            return {true, "OK", total};
        } catch (const std::exception& e) {
            return {false, e.what(), 0};
        }
    }
};

} // namespace usecases

// ============================================================
// Layer 3: INTERFACE ADAPTERS — controllers, presenters, gateways
// ============================================================
namespace adapters {

class InMemoryProductRepo : public usecases::ProductRepository {
    std::vector<entities::Product> products_;
public:
    void seed(entities::Product p) { products_.push_back(std::move(p)); }

    entities::Product* find(const std::string& name) override {
        for (auto& p : products_)
            if (p.name() == name) return &p;
        return nullptr;
    }

    void save(const entities::Product&) override {
        // In-memory — already mutated in place
    }
};

class ConsoleLogger : public usecases::Logger {
public:
    void log(const std::string& msg) override {
        std::cout << "  [Log] " << msg << "\n";
    }
};

// Controller: translates external requests to use case calls
class ShopController {
    usecases::PurchaseProduct& purchase_uc_;
public:
    explicit ShopController(usecases::PurchaseProduct& uc) : purchase_uc_(uc) {}

    void handle_purchase(const std::string& product, int qty) {
        std::cout << "[Controller] POST /purchase {" << product << ", qty=" << qty << "}\n";
        auto resp = purchase_uc_.execute({product, qty});
        if (resp.success)
            std::cout << "  -> 200 OK: total=$" << resp.total << "\n";
        else
            std::cout << "  -> 400 Error: " << resp.message << "\n";
    }
};

} // namespace adapters

// ============================================================
// Layer 4 (outermost): FRAMEWORKS & DRIVERS — main, DB, web
// ============================================================
int main() {
    // Assemble from outside in
    adapters::InMemoryProductRepo repo;
    repo.seed(entities::Product("Widget", 9.99, 10));
    repo.seed(entities::Product("Gadget", 49.99, 3));

    adapters::ConsoleLogger logger;
    usecases::PurchaseProduct purchase_uc(repo, logger);
    adapters::ShopController controller(purchase_uc);

    controller.handle_purchase("Widget", 2);
    controller.handle_purchase("Gadget", 5);  // not enough stock
    controller.handle_purchase("Unknown", 1); // not found

    return 0;
}
