#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <map>
#include <random>
#include <chrono>
#include <thread>
#include <cmath>

// Traditional Strategy Pattern - Sorting Algorithms
class SortStrategy {
public:
    virtual ~SortStrategy() = default;
    virtual void sort(std::vector<int>& data) = 0;
    virtual std::string getName() const = 0;
};

class BubbleSortStrategy : public SortStrategy {
public:
    void sort(std::vector<int>& data) override {
        std::cout << "Performing Bubble Sort..." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();

        for (size_t i = 0; i < data.size(); ++i) {
            for (size_t j = 0; j < data.size() - i - 1; ++j) {
                if (data[j] > data[j + 1]) {
                    std::swap(data[j], data[j + 1]);
                }
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "Bubble Sort completed in " << duration.count() << " microseconds" << std::endl;
    }

    std::string getName() const override {
        return "Bubble Sort";
    }
};

class QuickSortStrategy : public SortStrategy {
private:
    void quickSort(std::vector<int>& data, int low, int high) {
        if (low < high) {
            int pivot = partition(data, low, high);
            quickSort(data, low, pivot - 1);
            quickSort(data, pivot + 1, high);
        }
    }

    int partition(std::vector<int>& data, int low, int high) {
        int pivot = data[high];
        int i = low - 1;

        for (int j = low; j < high; ++j) {
            if (data[j] < pivot) {
                ++i;
                std::swap(data[i], data[j]);
            }
        }
        std::swap(data[i + 1], data[high]);
        return i + 1;
    }

public:
    void sort(std::vector<int>& data) override {
        std::cout << "Performing Quick Sort..." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();

        quickSort(data, 0, static_cast<int>(data.size()) - 1);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "Quick Sort completed in " << duration.count() << " microseconds" << std::endl;
    }

    std::string getName() const override {
        return "Quick Sort";
    }
};

class STLSortStrategy : public SortStrategy {
public:
    void sort(std::vector<int>& data) override {
        std::cout << "Performing STL Sort..." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();

        std::sort(data.begin(), data.end());

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "STL Sort completed in " << duration.count() << " microseconds" << std::endl;
    }

    std::string getName() const override {
        return "STL Sort";
    }
};

class SortContext {
private:
    std::unique_ptr<SortStrategy> strategy;

public:
    explicit SortContext(std::unique_ptr<SortStrategy> strategy)
        : strategy(std::move(strategy)) {}

    void setStrategy(std::unique_ptr<SortStrategy> newStrategy) {
        strategy = std::move(newStrategy);
    }

    void executeSort(std::vector<int>& data) {
        if (strategy) {
            std::cout << "Using strategy: " << strategy->getName() << std::endl;
            strategy->sort(data);
        } else {
            std::cout << "No sorting strategy set!" << std::endl;
        }
    }

    std::string getCurrentStrategy() const {
        return strategy ? strategy->getName() : "None";
    }
};

// Example 2: Payment Processing Strategies
class PaymentStrategy {
public:
    virtual ~PaymentStrategy() = default;
    virtual bool processPayment(double amount) = 0;
    virtual std::string getPaymentMethod() const = 0;
    virtual double getProcessingFee(double amount) const = 0;
};

class CreditCardPayment : public PaymentStrategy {
private:
    std::string cardNumber;
    std::string cardHolder;

public:
    CreditCardPayment(const std::string& cardNumber, const std::string& cardHolder)
        : cardNumber(cardNumber), cardHolder(cardHolder) {}

    bool processPayment(double amount) override {
        std::cout << "Processing credit card payment of $" << amount << std::endl;
        std::cout << "Card: ****" << cardNumber.substr(cardNumber.length() - 4)
                  << " (" << cardHolder << ")" << std::endl;

        // Simulate processing delay
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        std::cout << "Credit card payment successful!" << std::endl;
        return true;
    }

    std::string getPaymentMethod() const override {
        return "Credit Card";
    }

    double getProcessingFee(double amount) const override {
        return amount * 0.029; // 2.9% fee
    }
};

class PayPalPayment : public PaymentStrategy {
private:
    std::string email;

public:
    explicit PayPalPayment(const std::string& email) : email(email) {}

    bool processPayment(double amount) override {
        std::cout << "Processing PayPal payment of $" << amount << std::endl;
        std::cout << "PayPal account: " << email << std::endl;

        // Simulate processing delay
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        std::cout << "PayPal payment successful!" << std::endl;
        return true;
    }

    std::string getPaymentMethod() const override {
        return "PayPal";
    }

    double getProcessingFee(double amount) const override {
        return amount * 0.034; // 3.4% fee
    }
};

class BankTransferPayment : public PaymentStrategy {
private:
    std::string accountNumber;
    std::string routingNumber;

public:
    BankTransferPayment(const std::string& accountNumber, const std::string& routingNumber)
        : accountNumber(accountNumber), routingNumber(routingNumber) {}

    bool processPayment(double amount) override {
        std::cout << "Processing bank transfer payment of $" << amount << std::endl;
        std::cout << "Account: ****" << accountNumber.substr(accountNumber.length() - 4)
                  << " (Routing: " << routingNumber << ")" << std::endl;

        // Simulate longer processing delay for bank transfer
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        std::cout << "Bank transfer payment successful!" << std::endl;
        return true;
    }

    std::string getPaymentMethod() const override {
        return "Bank Transfer";
    }

    double getProcessingFee(double amount) const override {
        return 0.50; // Flat fee
    }
};

class PaymentProcessor {
private:
    std::unique_ptr<PaymentStrategy> paymentStrategy;

public:
    void setPaymentStrategy(std::unique_ptr<PaymentStrategy> strategy) {
        paymentStrategy = std::move(strategy);
    }

    bool processPayment(double amount) {
        if (!paymentStrategy) {
            std::cout << "No payment method selected!" << std::endl;
            return false;
        }

        double fee = paymentStrategy->getProcessingFee(amount);
        double total = amount + fee;

        std::cout << "\n--- Payment Processing ---" << std::endl;
        std::cout << "Method: " << paymentStrategy->getPaymentMethod() << std::endl;
        std::cout << "Amount: $" << amount << std::endl;
        std::cout << "Processing Fee: $" << fee << std::endl;
        std::cout << "Total: $" << total << std::endl;
        std::cout << "--------------------------" << std::endl;

        return paymentStrategy->processPayment(total);
    }

    std::string getCurrentPaymentMethod() const {
        return paymentStrategy ? paymentStrategy->getPaymentMethod() : "None";
    }
};

// Example 3: Modern C++ Strategy with std::function
using DiscountStrategy = std::function<double(double, int)>; // (price, quantity) -> discount

class ShoppingCart {
private:
    struct Item {
        std::string name;
        double price;
        int quantity;
    };

    std::vector<Item> items;
    DiscountStrategy discountStrategy;

public:
    void addItem(const std::string& name, double price, int quantity) {
        items.push_back({name, price, quantity});
    }

    void setDiscountStrategy(DiscountStrategy strategy) {
        discountStrategy = strategy;
    }

    double calculateTotal() const {
        double subtotal = 0.0;
        for (const auto& item : items) {
            subtotal += item.price * item.quantity;
        }

        double discount = 0.0;
        if (discountStrategy) {
            for (const auto& item : items) {
                discount += discountStrategy(item.price, item.quantity);
            }
        }

        return subtotal - discount;
    }

    void printReceipt() const {
        std::cout << "\n--- Shopping Cart Receipt ---" << std::endl;
        double subtotal = 0.0;
        double totalDiscount = 0.0;

        for (const auto& item : items) {
            double itemTotal = item.price * item.quantity;
            subtotal += itemTotal;

            std::cout << item.name << " x" << item.quantity
                      << " @ $" << item.price << " = $" << itemTotal << std::endl;

            if (discountStrategy) {
                double itemDiscount = discountStrategy(item.price, item.quantity);
                if (itemDiscount > 0) {
                    totalDiscount += itemDiscount;
                    std::cout << "  Discount: -$" << itemDiscount << std::endl;
                }
            }
        }

        std::cout << "----------------------------" << std::endl;
        std::cout << "Subtotal: $" << subtotal << std::endl;
        std::cout << "Total Discount: -$" << totalDiscount << std::endl;
        std::cout << "TOTAL: $" << (subtotal - totalDiscount) << std::endl;
        std::cout << "=============================" << std::endl;
    }

    void clearCart() {
        items.clear();
    }

    size_t getItemCount() const {
        return items.size();
    }
};

// Discount strategy factory
class DiscountStrategies {
public:
    static DiscountStrategy noDiscount() {
        return [](double price, int quantity) -> double {
            return 0.0;
        };
    }

    static DiscountStrategy percentageDiscount(double percentage) {
        return [percentage](double price, int quantity) -> double {
            return (price * quantity) * (percentage / 100.0);
        };
    }

    static DiscountStrategy bulkDiscount(int threshold, double discountPerItem) {
        return [threshold, discountPerItem](double price, int quantity) -> double {
            return quantity >= threshold ? quantity * discountPerItem : 0.0;
        };
    }

    static DiscountStrategy buyTwoGetOneFree() {
        return [](double price, int quantity) -> double {
            int freeItems = quantity / 3;
            return freeItems * price;
        };
    }

    static DiscountStrategy tieredDiscount() {
        return [](double price, int quantity) -> double {
            double itemTotal = price * quantity;
            if (itemTotal >= 100.0) return itemTotal * 0.15; // 15% for $100+
            if (itemTotal >= 50.0) return itemTotal * 0.10;  // 10% for $50+
            if (itemTotal >= 25.0) return itemTotal * 0.05;  // 5% for $25+
            return 0.0;
        };
    }
};

// Example 4: Game AI Strategy Pattern
class GameAIStrategy {
public:
    virtual ~GameAIStrategy() = default;
    virtual std::string makeMove(const std::vector<std::string>& gameState) = 0;
    virtual std::string getName() const = 0;
    virtual int getDifficulty() const = 0; // 1-10 scale
};

class AggressiveAI : public GameAIStrategy {
public:
    std::string makeMove(const std::vector<std::string>& gameState) override {
        std::cout << "AI thinking aggressively..." << std::endl;
        return "ATTACK_STRONGEST_ENEMY";
    }

    std::string getName() const override {
        return "Aggressive AI";
    }

    int getDifficulty() const override {
        return 7;
    }
};

class DefensiveAI : public GameAIStrategy {
public:
    std::string makeMove(const std::vector<std::string>& gameState) override {
        std::cout << "AI thinking defensively..." << std::endl;
        return "FORTIFY_WEAKEST_POSITION";
    }

    std::string getName() const override {
        return "Defensive AI";
    }

    int getDifficulty() const override {
        return 5;
    }
};

class BalancedAI : public GameAIStrategy {
public:
    std::string makeMove(const std::vector<std::string>& gameState) override {
        std::cout << "AI calculating balanced strategy..." << std::endl;
        // Simple decision logic
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(1, 3);

        switch (dist(rng)) {
            case 1: return "ATTACK_OPPORTUNITY_TARGET";
            case 2: return "DEFEND_KEY_POSITION";
            case 3: return "EXPAND_TERRITORY";
            default: return "WAIT";
        }
    }

    std::string getName() const override {
        return "Balanced AI";
    }

    int getDifficulty() const override {
        return 8;
    }
};

class GameEngine {
private:
    std::unique_ptr<GameAIStrategy> aiStrategy;
    std::vector<std::string> gameState;

public:
    void setAIStrategy(std::unique_ptr<GameAIStrategy> strategy) {
        aiStrategy = std::move(strategy);
        std::cout << "AI Strategy changed to: " << aiStrategy->getName()
                  << " (Difficulty: " << aiStrategy->getDifficulty() << "/10)" << std::endl;
    }

    void playTurn() {
        if (!aiStrategy) {
            std::cout << "No AI strategy set!" << std::endl;
            return;
        }

        // Update game state (simplified)
        gameState = {"PLAYER_HP_75", "ENEMY_HP_60", "RESOURCES_HIGH"};

        std::cout << "\n--- AI Turn ---" << std::endl;
        std::string move = aiStrategy->makeMove(gameState);
        std::cout << "AI Decision: " << move << std::endl;
        std::cout << "Turn completed." << std::endl;
    }

    std::string getCurrentAI() const {
        return aiStrategy ? aiStrategy->getName() : "None";
    }
};

int main() {
    std::cout << "=== Strategy Pattern Demo ===" << std::endl;

    // Example 1: Sorting Strategies
    std::cout << "\n1. Sorting Strategies:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    // Generate test data
    std::vector<int> data = {64, 34, 25, 12, 22, 11, 90, 88, 76, 50};
    std::cout << "Original data: ";
    for (int num : data) std::cout << num << " ";
    std::cout << std::endl;

    SortContext sorter(std::make_unique<BubbleSortStrategy>());

    // Test different sorting strategies
    std::vector<std::unique_ptr<SortStrategy>> strategies;
    strategies.push_back(std::make_unique<BubbleSortStrategy>());
    strategies.push_back(std::make_unique<QuickSortStrategy>());
    strategies.push_back(std::make_unique<STLSortStrategy>());

    for (auto& strategy : strategies) {
        std::vector<int> testData = data; // Copy original data
        sorter.setStrategy(std::move(strategy));
        sorter.executeSort(testData);

        std::cout << "Sorted data: ";
        for (size_t i = 0; i < std::min(testData.size(), size_t(10)); ++i) {
            std::cout << testData[i] << " ";
        }
        std::cout << std::endl << std::endl;
    }

    // Example 2: Payment Processing Strategies
    std::cout << "\n2. Payment Processing Strategies:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    PaymentProcessor processor;

    // Test different payment methods
    std::vector<std::unique_ptr<PaymentStrategy>> paymentMethods;
    paymentMethods.push_back(std::make_unique<CreditCardPayment>("1234567890123456", "John Doe"));
    paymentMethods.push_back(std::make_unique<PayPalPayment>("john.doe@example.com"));
    paymentMethods.push_back(std::make_unique<BankTransferPayment>("9876543210", "123456789"));

    double amount = 100.00;

    for (auto& method : paymentMethods) {
        processor.setPaymentStrategy(std::move(method));
        processor.processPayment(amount);
        std::cout << std::endl;
    }

    // Example 3: Modern C++ Strategy with std::function - Shopping Cart
    std::cout << "\n3. Shopping Cart with Discount Strategies:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    ShoppingCart cart;
    cart.addItem("Laptop", 999.99, 1);
    cart.addItem("Mouse", 29.99, 2);
    cart.addItem("Keyboard", 79.99, 1);

    // Test different discount strategies
    std::cout << "No discount:" << std::endl;
    cart.setDiscountStrategy(DiscountStrategies::noDiscount());
    cart.printReceipt();

    std::cout << "\n10% discount:" << std::endl;
    cart.setDiscountStrategy(DiscountStrategies::percentageDiscount(10.0));
    cart.printReceipt();

    std::cout << "\nBulk discount (2+ items get $5 off each):" << std::endl;
    cart.setDiscountStrategy(DiscountStrategies::bulkDiscount(2, 5.0));
    cart.printReceipt();

    std::cout << "\nBuy 2 get 1 free:" << std::endl;
    cart.setDiscountStrategy(DiscountStrategies::buyTwoGetOneFree());
    cart.printReceipt();

    std::cout << "\nTiered discount (5%/10%/15% based on item total):" << std::endl;
    cart.setDiscountStrategy(DiscountStrategies::tieredDiscount());
    cart.printReceipt();

    // Example 4: Game AI Strategies
    std::cout << "\n4. Game AI Strategies:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    GameEngine game;

    std::vector<std::unique_ptr<GameAIStrategy>> aiStrategies;
    aiStrategies.push_back(std::make_unique<AggressiveAI>());
    aiStrategies.push_back(std::make_unique<DefensiveAI>());
    aiStrategies.push_back(std::make_unique<BalancedAI>());

    for (auto& ai : aiStrategies) {
        game.setAIStrategy(std::move(ai));
        game.playTurn();
        std::cout << std::endl;
    }

    // Dynamic strategy switching based on game state
    std::cout << "\nDynamic AI Strategy Switching:" << std::endl;
    game.setAIStrategy(std::make_unique<AggressiveAI>());
    game.playTurn();

    std::cout << "\nPlayer health low, switching to defensive..." << std::endl;
    game.setAIStrategy(std::make_unique<DefensiveAI>());
    game.playTurn();

    std::cout << "\nGame balanced, switching to balanced strategy..." << std::endl;
    game.setAIStrategy(std::make_unique<BalancedAI>());
    game.playTurn();

    // Strategy Pattern Benefits
    std::cout << "\n\n5. Strategy Pattern Benefits:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "✓ Algorithms can be swapped at runtime" << std::endl;
    std::cout << "✓ Easy to add new strategies without modifying existing code" << std::endl;
    std::cout << "✓ Eliminates conditional statements for algorithm selection" << std::endl;
    std::cout << "✓ Each strategy is testable independently" << std::endl;
    std::cout << "✓ Promotes open/closed principle" << std::endl;
    std::cout << "✓ Modern C++ allows functional strategies with std::function" << std::endl;

    return 0;
}