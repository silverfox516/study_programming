# Strategy Pattern - C++ Implementation

## 개요

Strategy 패턴은 동일한 종류의 작업을 수행하는 알고리즘군을 정의하고, 각각을 캡슐화하여 상호 교체 가능하게 만드는 행위 패턴입니다. 알고리즘을 사용하는 클라이언트와 독립적으로 알고리즘을 변경할 수 있게 해줍니다.

## 구조

- **Strategy**: 모든 구체적 전략이 구현해야 하는 인터페이스
- **ConcreteStrategy**: Strategy 인터페이스를 구현하는 구체적 알고리즘
- **Context**: Strategy 객체를 사용하는 클래스

## C++ 구현

### 1. 정렬 알고리즘 Strategy

```cpp
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <chrono>

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

    std::string getName() const override { return "Bubble Sort"; }
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

        if (!data.empty()) {
            quickSort(data, 0, data.size() - 1);
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "Quick Sort completed in " << duration.count() << " microseconds" << std::endl;
    }

    std::string getName() const override { return "Quick Sort"; }
};

// Context 클래스
class Sorter {
private:
    std::unique_ptr<SortStrategy> strategy;

public:
    explicit Sorter(std::unique_ptr<SortStrategy> strategy)
        : strategy(std::move(strategy)) {}

    void setStrategy(std::unique_ptr<SortStrategy> newStrategy) {
        strategy = std::move(newStrategy);
    }

    void performSort(std::vector<int>& data) {
        if (strategy) {
            std::cout << "Using " << strategy->getName() << " strategy" << std::endl;
            strategy->sort(data);
        }
    }

    std::string getCurrentStrategy() const {
        return strategy ? strategy->getName() : "No strategy set";
    }
};
```

### 2. 할인 정책 Strategy

```cpp
class DiscountStrategy {
public:
    virtual ~DiscountStrategy() = default;
    virtual double calculateDiscount(double amount) = 0;
    virtual std::string getDescription() const = 0;
};

class NoDiscountStrategy : public DiscountStrategy {
public:
    double calculateDiscount(double amount) override {
        return 0.0;
    }

    std::string getDescription() const override {
        return "No Discount";
    }
};

class PercentageDiscountStrategy : public DiscountStrategy {
private:
    double percentage;

public:
    explicit PercentageDiscountStrategy(double percentage)
        : percentage(percentage) {}

    double calculateDiscount(double amount) override {
        return amount * (percentage / 100.0);
    }

    std::string getDescription() const override {
        return std::to_string(percentage) + "% Discount";
    }
};

class FixedAmountDiscountStrategy : public DiscountStrategy {
private:
    double fixedAmount;

public:
    explicit FixedAmountDiscountStrategy(double fixedAmount)
        : fixedAmount(fixedAmount) {}

    double calculateDiscount(double amount) override {
        return std::min(fixedAmount, amount);
    }

    std::string getDescription() const override {
        return "$" + std::to_string(fixedAmount) + " Off";
    }
};

class ShoppingCart {
private:
    std::vector<std::pair<std::string, double>> items;
    std::unique_ptr<DiscountStrategy> discountStrategy;

public:
    ShoppingCart() : discountStrategy(std::make_unique<NoDiscountStrategy>()) {}

    void addItem(const std::string& name, double price) {
        items.emplace_back(name, price);
    }

    void setDiscountStrategy(std::unique_ptr<DiscountStrategy> strategy) {
        discountStrategy = std::move(strategy);
    }

    double calculateTotal() {
        double subtotal = 0.0;
        for (const auto& [name, price] : items) {
            subtotal += price;
        }

        double discount = discountStrategy->calculateDiscount(subtotal);
        return subtotal - discount;
    }

    void printReceipt() {
        std::cout << "\\n=== Shopping Cart ===" << std::endl;
        double subtotal = 0.0;

        for (const auto& [name, price] : items) {
            std::cout << name << ": $" << std::fixed << std::setprecision(2) << price << std::endl;
            subtotal += price;
        }

        std::cout << "\\nSubtotal: $" << std::fixed << std::setprecision(2) << subtotal << std::endl;

        double discount = discountStrategy->calculateDiscount(subtotal);
        if (discount > 0) {
            std::cout << "Discount (" << discountStrategy->getDescription() << "): -$"
                     << std::fixed << std::setprecision(2) << discount << std::endl;
        }

        std::cout << "Total: $" << std::fixed << std::setprecision(2) << calculateTotal() << std::endl;
    }
};
```

### 3. 함수형 Strategy (Modern C++)

```cpp
#include <functional>

class PaymentProcessor {
public:
    using PaymentStrategy = std::function<bool(double, const std::string&)>;

private:
    PaymentStrategy strategy;

public:
    explicit PaymentProcessor(PaymentStrategy strategy) : strategy(std::move(strategy)) {}

    void setStrategy(PaymentStrategy newStrategy) {
        strategy = std::move(newStrategy);
    }

    bool processPayment(double amount, const std::string& details) {
        return strategy ? strategy(amount, details) : false;
    }
};

// Payment strategy functions
bool creditCardPayment(double amount, const std::string& cardNumber) {
    std::cout << "Processing credit card payment of $" << amount
              << " with card ending in " << cardNumber.substr(cardNumber.length() - 4) << std::endl;
    return true;
}

bool paypalPayment(double amount, const std::string& email) {
    std::cout << "Processing PayPal payment of $" << amount
              << " for account " << email << std::endl;
    return true;
}

bool bitcoinPayment(double amount, const std::string& walletAddress) {
    std::cout << "Processing Bitcoin payment of $" << amount
              << " to wallet " << walletAddress << std::endl;
    return true;
}
```

## 사용 예제

```cpp
int main() {
    // 1. 정렬 알고리즘 Strategy
    std::vector<int> data = {64, 34, 25, 12, 22, 11, 90, 5};

    Sorter sorter(std::make_unique<BubbleSortStrategy>());
    auto dataCopy1 = data;
    sorter.performSort(dataCopy1);

    sorter.setStrategy(std::make_unique<QuickSortStrategy>());
    auto dataCopy2 = data;
    sorter.performSort(dataCopy2);

    // 2. 할인 정책 Strategy
    ShoppingCart cart;
    cart.addItem("Laptop", 1000.00);
    cart.addItem("Mouse", 25.00);
    cart.addItem("Keyboard", 75.00);

    // 할인 없음
    cart.printReceipt();

    // 10% 할인
    cart.setDiscountStrategy(std::make_unique<PercentageDiscountStrategy>(10.0));
    cart.printReceipt();

    // $50 고정 할인
    cart.setDiscountStrategy(std::make_unique<FixedAmountDiscountStrategy>(50.0));
    cart.printReceipt();

    // 3. 함수형 Strategy
    PaymentProcessor processor(creditCardPayment);
    processor.processPayment(100.00, "1234567890123456");

    processor.setStrategy(paypalPayment);
    processor.processPayment(50.00, "user@example.com");

    // 람다를 사용한 즉석 Strategy
    processor.setStrategy([](double amount, const std::string& details) {
        std::cout << "Processing bank transfer of $" << amount
                  << " to account " << details << std::endl;
        return true;
    });
    processor.processPayment(200.00, "ACC123456789");

    return 0;
}
```

## C++의 장점

1. **함수 객체 지원**: `std::function`으로 유연한 Strategy 구현
2. **람다 표현식**: 간단한 Strategy를 즉석에서 정의
3. **템플릿**: 타입 안전한 제네릭 Strategy
4. **스마트 포인터**: 자동 메모리 관리

## 적용 상황

1. **알고리즘 선택**: 정렬, 검색, 압축 알고리즘
2. **비즈니스 로직**: 할인 정책, 결제 방식, 배송 방법
3. **렌더링**: 그래픽 렌더링 방식
4. **데이터 검증**: 다양한 유효성 검사 규칙
5. **게임 AI**: 캐릭터 행동 패턴

Strategy 패턴은 개방-폐쇄 원칙을 구현하여 새로운 알고리즘을 쉽게 추가할 수 있게 해주는 핵심적인 패턴입니다.