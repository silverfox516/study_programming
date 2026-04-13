// Domain Service
// Business logic that doesn't belong to a single entity or value object.
// Stateless operations spanning multiple aggregates.

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

// ============================================================
// Domain entities
// ============================================================
class Account {
    std::string owner_;
    double balance_;
    std::string currency_;
public:
    Account(std::string owner, double balance, std::string currency)
        : owner_(std::move(owner)), balance_(balance), currency_(std::move(currency)) {}

    const std::string& owner() const { return owner_; }
    double balance() const { return balance_; }
    const std::string& currency() const { return currency_; }

    void debit(double amount) {
        if (amount > balance_)
            throw std::runtime_error("Insufficient funds in " + owner_ + "'s account");
        balance_ -= amount;
    }

    void credit(double amount) {
        balance_ += amount;
    }
};

class ExchangeRate {
    std::string from_, to_;
    double rate_;
public:
    ExchangeRate(std::string from, std::string to, double rate)
        : from_(std::move(from)), to_(std::move(to)), rate_(rate) {}

    double convert(double amount) const { return amount * rate_; }
    const std::string& from() const { return from_; }
    const std::string& to() const { return to_; }
    double rate() const { return rate_; }
};

// ============================================================
// Domain Service: transfer logic spans two aggregates
// ============================================================
class TransferService {
public:
    // This logic doesn't belong to Account — it coordinates TWO accounts
    void transfer(Account& from, Account& to, double amount) {
        if (&from == &to)
            throw std::runtime_error("Cannot transfer to the same account");

        std::cout << "[TransferService] " << from.owner() << " -> "
                  << to.owner() << ": $" << amount << "\n";

        from.debit(amount);
        to.credit(amount);

        std::cout << "  " << from.owner() << " balance: $" << from.balance() << "\n";
        std::cout << "  " << to.owner() << " balance: $" << to.balance() << "\n";
    }

    // Cross-currency transfer — even more clearly a service concern
    void transfer_with_exchange(Account& from, Account& to,
                                 double amount, const ExchangeRate& rate) {
        if (from.currency() != rate.from() || to.currency() != rate.to())
            throw std::runtime_error("Exchange rate currency mismatch");

        double converted = rate.convert(amount);
        std::cout << "[TransferService] Cross-currency: " << amount << " "
                  << from.currency() << " = " << converted << " " << to.currency()
                  << " (rate: " << rate.rate() << ")\n";

        from.debit(amount);
        to.credit(converted);

        std::cout << "  " << from.owner() << ": " << from.balance() << " " << from.currency() << "\n";
        std::cout << "  " << to.owner() << ": " << to.balance() << " " << to.currency() << "\n";
    }
};

// ============================================================
// Another Domain Service: pricing spans Product + Customer
// ============================================================
struct Product {
    std::string name;
    double base_price;
};

struct Customer {
    std::string name;
    bool is_premium;
    int loyalty_years;
};

class PricingService {
public:
    double calculate_price(const Product& product, const Customer& customer) {
        double price = product.base_price;

        // Discount rules span both entities — neither owns this logic
        if (customer.is_premium)
            price *= 0.9; // 10% premium discount

        if (customer.loyalty_years >= 5)
            price *= 0.95; // 5% loyalty discount

        std::cout << "[PricingService] " << product.name << " for " << customer.name
                  << ": base=$" << product.base_price << " -> final=$" << price << "\n";
        return price;
    }
};

int main() {
    std::cout << "=== Transfer Domain Service ===\n";
    {
        Account alice("Alice", 1000.0, "USD");
        Account bob("Bob", 500.0, "USD");

        TransferService transfer_svc;
        transfer_svc.transfer(alice, bob, 200.0);
    }

    std::cout << "\n=== Cross-Currency Transfer ===\n";
    {
        Account alice("Alice", 1000.0, "USD");
        Account yuki("Yuki", 50000.0, "JPY");
        ExchangeRate usd_jpy("USD", "JPY", 150.0);

        TransferService transfer_svc;
        transfer_svc.transfer_with_exchange(alice, yuki, 100.0, usd_jpy);
    }

    std::cout << "\n=== Pricing Domain Service ===\n";
    {
        Product laptop{"Laptop", 999.99};
        Customer regular{"Bob", false, 1};
        Customer vip{"Alice", true, 7};

        PricingService pricing;
        pricing.calculate_price(laptop, regular);
        pricing.calculate_price(laptop, vip);
    }

    return 0;
}
