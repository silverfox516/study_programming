# Event Sourcing Pattern - C++ Implementation

## 개요

Event Sourcing 패턴은 애플리케이션의 모든 상태 변경을 불변 이벤트의 시퀀스로 저장하는 아키텍처 패턴입니다. 현재 상태를 직접 저장하는 대신, 상태에 대한 모든 변경 사항을 이벤트로 기록하고, 필요할 때 이벤트들을 재생하여 현재 상태를 복원합니다. 이는 완전한 감사 추적, 시간 여행, 그리고 복잡한 비즈니스 로직 구현을 가능하게 합니다.

## 구조

Event Sourcing 패턴의 주요 구성 요소:
- **Event**: 상태 변경을 나타내는 불변 객체
- **Aggregate**: 비즈니스 엔티티의 현재 상태
- **Command**: 상태 변경 요청
- **EventStore**: 이벤트를 저장하는 저장소
- **CommandHandler**: 커맨드를 처리하고 이벤트를 생성
- **Projection**: 이벤트로부터 생성되는 읽기 모델

## C++ 구현

### 1. 기본 Event 클래스

```cpp
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <chrono>
#include <unordered_map>

class UUID {
private:
    std::string value_;
    static int counter_;

public:
    UUID() : value_("UUID-" + std::to_string(++counter_)) {}
    explicit UUID(const std::string& value) : value_(value) {}

    const std::string& toString() const { return value_; }
    bool operator==(const UUID& other) const { return value_ == other.value_; }
};

class Event {
private:
    UUID aggregateId_;
    std::chrono::system_clock::time_point timestamp_;

public:
    Event(const UUID& aggregateId)
        : aggregateId_(aggregateId), timestamp_(std::chrono::system_clock::now()) {}

    virtual ~Event() = default;

    const UUID& getAggregateId() const { return aggregateId_; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp_; }

    virtual std::string getEventType() const = 0;
    virtual std::string toString() const = 0;

    std::string getFormattedTimestamp() const {
        auto time_t = std::chrono::system_clock::to_time_t(timestamp_);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};
```

### 2. 구체적인 이벤트 클래스들

```cpp
class AccountOpenedEvent : public Event {
private:
    std::string ownerName_;
    double initialBalance_;

public:
    AccountOpenedEvent(const UUID& accountId, const std::string& ownerName, double initialBalance)
        : Event(accountId), ownerName_(ownerName), initialBalance_(initialBalance) {}

    std::string getEventType() const override { return "AccountOpened"; }
    const std::string& getOwnerName() const { return ownerName_; }
    double getInitialBalance() const { return initialBalance_; }

    std::string toString() const override {
        return "AccountOpened{accountId=" + getAggregateId().toString() +
               ", owner=" + ownerName_ + ", balance=" + std::to_string(initialBalance_) + "}";
    }
};

class MoneyDepositedEvent : public Event {
private:
    double amount_;

public:
    MoneyDepositedEvent(const UUID& accountId, double amount)
        : Event(accountId), amount_(amount) {}

    std::string getEventType() const override { return "MoneyDeposited"; }
    double getAmount() const { return amount_; }

    std::string toString() const override {
        return "MoneyDeposited{accountId=" + getAggregateId().toString() +
               ", amount=" + std::to_string(amount_) + "}";
    }
};

class MoneyWithdrawnEvent : public Event {
private:
    double amount_;

public:
    MoneyWithdrawnEvent(const UUID& accountId, double amount)
        : Event(accountId), amount_(amount) {}

    std::string getEventType() const override { return "MoneyWithdrawn"; }
    double getAmount() const { return amount_; }

    std::string toString() const override {
        return "MoneyWithdrawn{accountId=" + getAggregateId().toString() +
               ", amount=" + std::to_string(amount_) + "}";
    }
};
```

### 3. Aggregate 클래스

```cpp
class BankAccount {
private:
    UUID id_;
    std::string ownerName_;
    double balance_;
    bool isClosed_;
    size_t version_;

public:
    BankAccount() : balance_(0.0), isClosed_(false), version_(0) {}
    BankAccount(const UUID& id) : id_(id), balance_(0.0), isClosed_(false), version_(0) {}

    // Getters
    const UUID& getId() const { return id_; }
    const std::string& getOwnerName() const { return ownerName_; }
    double getBalance() const { return balance_; }
    bool isClosed() const { return isClosed_; }
    size_t getVersion() const { return version_; }

    // Apply an event to update the state
    void applyEvent(const Event& event) {
        if (auto accountOpened = dynamic_cast<const AccountOpenedEvent*>(&event)) {
            id_ = accountOpened->getAggregateId();
            ownerName_ = accountOpened->getOwnerName();
            balance_ = accountOpened->getInitialBalance();
            isClosed_ = false;
        } else if (auto moneyDeposited = dynamic_cast<const MoneyDepositedEvent*>(&event)) {
            balance_ += moneyDeposited->getAmount();
        } else if (auto moneyWithdrawn = dynamic_cast<const MoneyWithdrawnEvent*>(&event)) {
            balance_ -= moneyWithdrawn->getAmount();
        } else if (auto accountClosed = dynamic_cast<const AccountClosedEvent*>(&event)) {
            isClosed_ = true;
        }
        version_++;
    }

    // Reconstruct state from events
    static BankAccount fromEvents(const std::vector<std::unique_ptr<Event>>& events) {
        BankAccount account;
        for (const auto& event : events) {
            account.applyEvent(*event);
        }
        return account;
    }
};
```

### 4. Command 클래스들

```cpp
class Command {
public:
    virtual ~Command() = default;
    virtual std::string getCommandType() const = 0;
};

class OpenAccountCommand : public Command {
private:
    UUID accountId_;
    std::string ownerName_;
    double initialBalance_;

public:
    OpenAccountCommand(const UUID& accountId, const std::string& ownerName, double initialBalance)
        : accountId_(accountId), ownerName_(ownerName), initialBalance_(initialBalance) {}

    std::string getCommandType() const override { return "OpenAccount"; }
    const UUID& getAccountId() const { return accountId_; }
    const std::string& getOwnerName() const { return ownerName_; }
    double getInitialBalance() const { return initialBalance_; }
};

class DepositMoneyCommand : public Command {
private:
    UUID accountId_;
    double amount_;

public:
    DepositMoneyCommand(const UUID& accountId, double amount)
        : accountId_(accountId), amount_(amount) {}

    std::string getCommandType() const override { return "DepositMoney"; }
    const UUID& getAccountId() const { return accountId_; }
    double getAmount() const { return amount_; }
};
```

### 5. EventStore 인터페이스와 구현

```cpp
class EventStore {
public:
    virtual ~EventStore() = default;
    virtual void appendEvents(const UUID& aggregateId, const std::vector<std::unique_ptr<Event>>& events) = 0;
    virtual std::vector<std::unique_ptr<Event>> getEvents(const UUID& aggregateId) const = 0;
    virtual std::vector<std::unique_ptr<Event>> getAllEvents() const = 0;
    virtual size_t getEventCount() const = 0;
};

class InMemoryEventStore : public EventStore {
private:
    std::unordered_map<UUID, std::vector<std::unique_ptr<Event>>> eventsByAggregate_;
    std::vector<std::unique_ptr<Event>> allEvents_;

    std::unique_ptr<Event> cloneEvent(const Event& event) const {
        if (auto accountOpened = dynamic_cast<const AccountOpenedEvent*>(&event)) {
            return std::make_unique<AccountOpenedEvent>(
                accountOpened->getAggregateId(),
                accountOpened->getOwnerName(),
                accountOpened->getInitialBalance()
            );
        } else if (auto moneyDeposited = dynamic_cast<const MoneyDepositedEvent*>(&event)) {
            return std::make_unique<MoneyDepositedEvent>(
                moneyDeposited->getAggregateId(),
                moneyDeposited->getAmount()
            );
        }
        // ... other event types
        return nullptr;
    }

public:
    void appendEvents(const UUID& aggregateId, const std::vector<std::unique_ptr<Event>>& events) override {
        auto& aggregateEvents = eventsByAggregate_[aggregateId];

        for (const auto& event : events) {
            auto clonedForAggregate = cloneEvent(*event);
            if (clonedForAggregate) {
                aggregateEvents.push_back(std::move(clonedForAggregate));
            }

            auto clonedForAll = cloneEvent(*event);
            if (clonedForAll) {
                allEvents_.push_back(std::move(clonedForAll));
            }
        }
    }

    std::vector<std::unique_ptr<Event>> getEvents(const UUID& aggregateId) const override {
        std::vector<std::unique_ptr<Event>> result;

        auto it = eventsByAggregate_.find(aggregateId);
        if (it != eventsByAggregate_.end()) {
            for (const auto& event : it->second) {
                auto cloned = cloneEvent(*event);
                if (cloned) {
                    result.push_back(std::move(cloned));
                }
            }
        }
        return result;
    }
};
```

### 6. Command Handler

```cpp
class BankAccountCommandHandler {
private:
    std::unique_ptr<EventStore> eventStore_;

public:
    explicit BankAccountCommandHandler(std::unique_ptr<EventStore> eventStore)
        : eventStore_(std::move(eventStore)) {}

    std::vector<std::unique_ptr<Event>> handleCommand(const Command& command) {
        if (auto openAccount = dynamic_cast<const OpenAccountCommand*>(&command)) {
            return handleOpenAccount(*openAccount);
        } else if (auto depositMoney = dynamic_cast<const DepositMoneyCommand*>(&command)) {
            return handleDepositMoney(*depositMoney);
        }
        // ... handle other commands
        throw std::runtime_error("Unknown command type");
    }

private:
    std::vector<std::unique_ptr<Event>> handleOpenAccount(const OpenAccountCommand& command) {
        // Validate command
        if (command.getInitialBalance() < 0.0) {
            throw std::runtime_error("Initial balance cannot be negative");
        }

        // Check if account already exists
        auto existingEvents = eventStore_->getEvents(command.getAccountId());
        if (!existingEvents.empty()) {
            throw std::runtime_error("Account already exists");
        }

        std::vector<std::unique_ptr<Event>> events;
        events.push_back(std::make_unique<AccountOpenedEvent>(
            command.getAccountId(),
            command.getOwnerName(),
            command.getInitialBalance()
        ));

        eventStore_->appendEvents(command.getAccountId(), events);
        return events;
    }

    std::vector<std::unique_ptr<Event>> handleDepositMoney(const DepositMoneyCommand& command) {
        // Validate command
        if (command.getAmount() <= 0.0) {
            throw std::runtime_error("Deposit amount must be positive");
        }

        // Load current state
        auto events = eventStore_->getEvents(command.getAccountId());
        if (events.empty()) {
            throw std::runtime_error("Account does not exist");
        }

        auto account = BankAccount::fromEvents(events);
        if (account.isClosed()) {
            throw std::runtime_error("Cannot deposit to closed account");
        }

        std::vector<std::unique_ptr<Event>> newEvents;
        newEvents.push_back(std::make_unique<MoneyDepositedEvent>(
            command.getAccountId(),
            command.getAmount()
        ));

        eventStore_->appendEvents(command.getAccountId(), newEvents);
        return newEvents;
    }
};
```

### 7. Projection (Read Model)

```cpp
class AccountSummary {
public:
    UUID id;
    std::string ownerName;
    double balance = 0.0;
    double totalDeposits = 0.0;
    double totalWithdrawals = 0.0;
    size_t transactionCount = 0;
    bool isClosed = false;

    static AccountSummary fromEvents(const std::vector<std::unique_ptr<Event>>& events) {
        AccountSummary summary;

        for (const auto& event : events) {
            if (auto accountOpened = dynamic_cast<const AccountOpenedEvent*>(event.get())) {
                summary.id = accountOpened->getAggregateId();
                summary.ownerName = accountOpened->getOwnerName();
                summary.balance = accountOpened->getInitialBalance();
                summary.totalDeposits = accountOpened->getInitialBalance();
                summary.transactionCount++;
            } else if (auto moneyDeposited = dynamic_cast<const MoneyDepositedEvent*>(event.get())) {
                summary.balance += moneyDeposited->getAmount();
                summary.totalDeposits += moneyDeposited->getAmount();
                summary.transactionCount++;
            } else if (auto moneyWithdrawn = dynamic_cast<const MoneyWithdrawnEvent*>(event.get())) {
                summary.balance -= moneyWithdrawn->getAmount();
                summary.totalWithdrawals += moneyWithdrawn->getAmount();
                summary.transactionCount++;
            }
        }
        return summary;
    }
};
```

## 사용 예제

```cpp
int main() {
    // Create event store and command handler
    auto eventStore = std::make_unique<InMemoryEventStore>();
    auto* eventStorePtr = eventStore.get();
    auto commandHandler = std::make_unique<BankAccountCommandHandler>(std::move(eventStore));

    // Create bank account
    UUID accountId;
    try {
        commandHandler->handleCommand(
            OpenAccountCommand(accountId, "Alice Johnson", 1000.0)
        );
        std::cout << "Account opened for Alice Johnson\n";

        // Perform transactions
        commandHandler->handleCommand(DepositMoneyCommand(accountId, 250.0));
        commandHandler->handleCommand(WithdrawMoneyCommand(accountId, 100.0));

        // Get current state
        auto account = commandHandler->getAccountState(accountId);
        if (account) {
            std::cout << "Current balance: $" << account->getBalance() << std::endl;
        }

        // Create projection
        auto events = eventStorePtr->getEvents(accountId);
        auto summary = AccountSummary::fromEvents(events);
        std::cout << "Total deposits: $" << summary.totalDeposits << std::endl;
        std::cout << "Transaction count: " << summary.transactionCount << std::endl;

    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }

    return 0;
}
```

## C++의 장점

1. **RAII**: 자동 리소스 관리로 메모리 누수 방지
2. **스마트 포인터**: `unique_ptr`로 이벤트 소유권 명확화
3. **타입 시스템**: 컴파일 타임 타입 검사
4. **다형성**: 가상 함수를 통한 이벤트 처리
5. **STL 컨테이너**: 효율적인 이벤트 저장 및 조회

## 적용 상황

1. **금융 시스템**: 모든 거래 내역의 완벽한 감사 추적
2. **전자상거래**: 주문, 결제, 배송 상태 변경 이력
3. **게임**: 플레이어 행동과 게임 상태 변화
4. **의료 시스템**: 환자 치료 기록과 의료 데이터
5. **협업 도구**: 문서 편집 이력과 버전 관리

Event Sourcing은 완전한 감사 추적, 시간 여행 쿼리, 그리고 복잡한 비즈니스 로직의 구현을 가능하게 하는 강력한 아키텍처 패턴입니다.