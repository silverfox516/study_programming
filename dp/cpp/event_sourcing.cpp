#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <chrono>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <variant>
#include <optional>

// Utility for UUID generation (simplified)
class UUID {
private:
    std::string value_;
    static int counter_;

public:
    UUID() : value_("UUID-" + std::to_string(++counter_)) {}
    explicit UUID(const std::string& value) : value_(value) {}

    const std::string& toString() const { return value_; }
    bool operator==(const UUID& other) const { return value_ == other.value_; }
    bool operator<(const UUID& other) const { return value_ < other.value_; }
};

int UUID::counter_ = 0;

// Hash function for UUID to use in unordered_map
namespace std {
    template<>
    struct hash<UUID> {
        size_t operator()(const UUID& uuid) const {
            return std::hash<std::string>{}(uuid.toString());
        }
    };
}

// Base Event class
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

// Bank Account Events
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

class AccountClosedEvent : public Event {
private:
    double finalBalance_;

public:
    AccountClosedEvent(const UUID& accountId, double finalBalance)
        : Event(accountId), finalBalance_(finalBalance) {}

    std::string getEventType() const override { return "AccountClosed"; }
    double getFinalBalance() const { return finalBalance_; }

    std::string toString() const override {
        return "AccountClosed{accountId=" + getAggregateId().toString() +
               ", finalBalance=" + std::to_string(finalBalance_) + "}";
    }
};

// Aggregate (current state)
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

    std::string toString() const {
        return "BankAccount{id=" + id_.toString() +
               ", owner=" + ownerName_ +
               ", balance=" + std::to_string(balance_) +
               ", closed=" + (isClosed_ ? "true" : "false") +
               ", version=" + std::to_string(version_) + "}";
    }
};

// Commands (intent to change state)
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

class WithdrawMoneyCommand : public Command {
private:
    UUID accountId_;
    double amount_;

public:
    WithdrawMoneyCommand(const UUID& accountId, double amount)
        : accountId_(accountId), amount_(amount) {}

    std::string getCommandType() const override { return "WithdrawMoney"; }
    const UUID& getAccountId() const { return accountId_; }
    double getAmount() const { return amount_; }
};

class CloseAccountCommand : public Command {
private:
    UUID accountId_;

public:
    explicit CloseAccountCommand(const UUID& accountId) : accountId_(accountId) {}

    std::string getCommandType() const override { return "CloseAccount"; }
    const UUID& getAccountId() const { return accountId_; }
};

// Event Store interface
class EventStore {
public:
    virtual ~EventStore() = default;
    virtual void appendEvents(const UUID& aggregateId, const std::vector<std::unique_ptr<Event>>& events) = 0;
    virtual std::vector<std::unique_ptr<Event>> getEvents(const UUID& aggregateId) const = 0;
    virtual std::vector<std::unique_ptr<Event>> getAllEvents() const = 0;
    virtual size_t getEventCount() const = 0;
};

// In-memory event store implementation
class InMemoryEventStore : public EventStore {
private:
    std::unordered_map<UUID, std::vector<std::unique_ptr<Event>>> eventsByAggregate_;
    std::vector<std::unique_ptr<Event>> allEvents_;

    // Helper to clone events
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
        } else if (auto moneyWithdrawn = dynamic_cast<const MoneyWithdrawnEvent*>(&event)) {
            return std::make_unique<MoneyWithdrawnEvent>(
                moneyWithdrawn->getAggregateId(),
                moneyWithdrawn->getAmount()
            );
        } else if (auto accountClosed = dynamic_cast<const AccountClosedEvent*>(&event)) {
            return std::make_unique<AccountClosedEvent>(
                accountClosed->getAggregateId(),
                accountClosed->getFinalBalance()
            );
        }
        return nullptr;
    }

public:
    void appendEvents(const UUID& aggregateId, const std::vector<std::unique_ptr<Event>>& events) override {
        auto& aggregateEvents = eventsByAggregate_[aggregateId];

        for (const auto& event : events) {
            // Clone event for aggregate-specific storage
            auto clonedForAggregate = cloneEvent(*event);
            if (clonedForAggregate) {
                aggregateEvents.push_back(std::move(clonedForAggregate));
            }

            // Clone event for global storage
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

    std::vector<std::unique_ptr<Event>> getAllEvents() const override {
        std::vector<std::unique_ptr<Event>> result;

        for (const auto& event : allEvents_) {
            auto cloned = cloneEvent(*event);
            if (cloned) {
                result.push_back(std::move(cloned));
            }
        }

        // Sort by timestamp
        std::sort(result.begin(), result.end(),
                 [](const std::unique_ptr<Event>& a, const std::unique_ptr<Event>& b) {
                     return a->getTimestamp() < b->getTimestamp();
                 });

        return result;
    }

    size_t getEventCount() const override {
        return allEvents_.size();
    }
};

// Command Handler
class BankAccountCommandHandler {
private:
    std::unique_ptr<EventStore> eventStore_;

public:
    explicit BankAccountCommandHandler(std::unique_ptr<EventStore> eventStore)
        : eventStore_(std::move(eventStore)) {}

    std::vector<std::unique_ptr<Event>> handleCommand(const Command& command) {
        std::vector<std::unique_ptr<Event>> newEvents;

        if (auto openAccount = dynamic_cast<const OpenAccountCommand*>(&command)) {
            return handleOpenAccount(*openAccount);
        } else if (auto depositMoney = dynamic_cast<const DepositMoneyCommand*>(&command)) {
            return handleDepositMoney(*depositMoney);
        } else if (auto withdrawMoney = dynamic_cast<const WithdrawMoneyCommand*>(&command)) {
            return handleWithdrawMoney(*withdrawMoney);
        } else if (auto closeAccount = dynamic_cast<const CloseAccountCommand*>(&command)) {
            return handleCloseAccount(*closeAccount);
        }

        throw std::runtime_error("Unknown command type");
    }

    std::optional<BankAccount> getAccountState(const UUID& accountId) {
        auto events = eventStore_->getEvents(accountId);
        if (events.empty()) {
            return std::nullopt;
        }
        return BankAccount::fromEvents(events);
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

    std::vector<std::unique_ptr<Event>> handleWithdrawMoney(const WithdrawMoneyCommand& command) {
        // Validate command
        if (command.getAmount() <= 0.0) {
            throw std::runtime_error("Withdrawal amount must be positive");
        }

        // Load current state
        auto events = eventStore_->getEvents(command.getAccountId());
        if (events.empty()) {
            throw std::runtime_error("Account does not exist");
        }

        auto account = BankAccount::fromEvents(events);
        if (account.isClosed()) {
            throw std::runtime_error("Cannot withdraw from closed account");
        }

        if (account.getBalance() < command.getAmount()) {
            throw std::runtime_error("Insufficient funds");
        }

        std::vector<std::unique_ptr<Event>> newEvents;
        newEvents.push_back(std::make_unique<MoneyWithdrawnEvent>(
            command.getAccountId(),
            command.getAmount()
        ));

        eventStore_->appendEvents(command.getAccountId(), newEvents);
        return newEvents;
    }

    std::vector<std::unique_ptr<Event>> handleCloseAccount(const CloseAccountCommand& command) {
        // Load current state
        auto events = eventStore_->getEvents(command.getAccountId());
        if (events.empty()) {
            throw std::runtime_error("Account does not exist");
        }

        auto account = BankAccount::fromEvents(events);
        if (account.isClosed()) {
            throw std::runtime_error("Account is already closed");
        }

        std::vector<std::unique_ptr<Event>> newEvents;
        newEvents.push_back(std::make_unique<AccountClosedEvent>(
            command.getAccountId(),
            account.getBalance()
        ));

        eventStore_->appendEvents(command.getAccountId(), newEvents);
        return newEvents;
    }
};

// Read model (projection)
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
            } else if (auto accountClosed = dynamic_cast<const AccountClosedEvent*>(event.get())) {
                summary.isClosed = true;
                summary.transactionCount++;
            }
        }

        return summary;
    }

    std::string toString() const {
        return "AccountSummary{id=" + id.toString() +
               ", owner=" + ownerName +
               ", balance=" + std::to_string(balance) +
               ", totalDeposits=" + std::to_string(totalDeposits) +
               ", totalWithdrawals=" + std::to_string(totalWithdrawals) +
               ", transactions=" + std::to_string(transactionCount) +
               ", closed=" + (isClosed ? "true" : "false") + "}";
    }
};

int main() {
    std::cout << "=== Event Sourcing Pattern Demo ===\n\n";

    try {
        // Create event store and command handler
        auto eventStore = std::make_unique<InMemoryEventStore>();
        auto* eventStorePtr = eventStore.get(); // Keep pointer for direct access
        auto commandHandler = std::make_unique<BankAccountCommandHandler>(std::move(eventStore));

        // Create some bank accounts
        UUID account1Id;
        UUID account2Id;

        std::cout << "1. Opening bank accounts:\n";

        // Open first account
        try {
            auto events = commandHandler->handleCommand(
                OpenAccountCommand(account1Id, "Alice Johnson", 1000.0)
            );
            std::cout << "✓ Account opened for Alice Johnson (ID: " << account1Id.toString() << ")\n";
        } catch (const std::exception& e) {
            std::cout << "✗ Failed to open Alice's account: " << e.what() << "\n";
        }

        // Open second account
        try {
            auto events = commandHandler->handleCommand(
                OpenAccountCommand(account2Id, "Bob Smith", 500.0)
            );
            std::cout << "✓ Account opened for Bob Smith (ID: " << account2Id.toString() << ")\n";
        } catch (const std::exception& e) {
            std::cout << "✗ Failed to open Bob's account: " << e.what() << "\n";
        }

        std::cout << "\n2. Performing transactions:\n";

        // Alice deposits money
        try {
            commandHandler->handleCommand(DepositMoneyCommand(account1Id, 250.0));
            std::cout << "✓ Alice deposited $250\n";
        } catch (const std::exception& e) {
            std::cout << "✗ Alice's deposit failed: " << e.what() << "\n";
        }

        // Bob withdraws money
        try {
            commandHandler->handleCommand(WithdrawMoneyCommand(account2Id, 100.0));
            std::cout << "✓ Bob withdrew $100\n";
        } catch (const std::exception& e) {
            std::cout << "✗ Bob's withdrawal failed: " << e.what() << "\n";
        }

        // Alice makes another deposit
        try {
            commandHandler->handleCommand(DepositMoneyCommand(account1Id, 500.0));
            std::cout << "✓ Alice deposited $500\n";
        } catch (const std::exception& e) {
            std::cout << "✗ Alice's second deposit failed: " << e.what() << "\n";
        }

        std::cout << "\n3. Current account states:\n";

        // Show current states
        auto account1State = commandHandler->getAccountState(account1Id);
        if (account1State) {
            std::cout << "Alice's account: $" << std::fixed << std::setprecision(2)
                     << account1State->getBalance() << " ("
                     << (account1State->isClosed() ? "Closed" : "Open") << ")\n";
        }

        auto account2State = commandHandler->getAccountState(account2Id);
        if (account2State) {
            std::cout << "Bob's account: $" << std::fixed << std::setprecision(2)
                     << account2State->getBalance() << " ("
                     << (account2State->isClosed() ? "Closed" : "Open") << ")\n";
        }

        std::cout << "\n4. Event history (Alice's account):\n";
        auto aliceEvents = eventStorePtr->getEvents(account1Id);
        for (size_t i = 0; i < aliceEvents.size(); ++i) {
            const auto& event = aliceEvents[i];
            std::cout << "Event " << (i + 1) << ": " << event->getEventType()
                     << " at " << event->getFormattedTimestamp() << "\n";
            std::cout << "  " << event->toString() << "\n";
        }

        std::cout << "\n5. Account summary (projection):\n";
        auto aliceSummary = AccountSummary::fromEvents(aliceEvents);
        std::cout << "Alice's Summary:\n";
        std::cout << "  Total deposits: $" << std::fixed << std::setprecision(2) << aliceSummary.totalDeposits << "\n";
        std::cout << "  Total withdrawals: $" << std::fixed << std::setprecision(2) << aliceSummary.totalWithdrawals << "\n";
        std::cout << "  Transaction count: " << aliceSummary.transactionCount << "\n";
        std::cout << "  Current balance: $" << std::fixed << std::setprecision(2) << aliceSummary.balance << "\n";

        std::cout << "\n6. Testing business rules:\n";

        // Try to withdraw more than balance
        try {
            commandHandler->handleCommand(WithdrawMoneyCommand(account2Id, 1000.0));
            std::cout << "✓ Large withdrawal succeeded\n";
        } catch (const std::exception& e) {
            std::cout << "✗ Large withdrawal failed: " << e.what() << "\n";
        }

        // Close Bob's account
        try {
            commandHandler->handleCommand(CloseAccountCommand(account2Id));
            std::cout << "✓ Bob's account closed\n";
        } catch (const std::exception& e) {
            std::cout << "✗ Failed to close Bob's account: " << e.what() << "\n";
        }

        // Try to deposit to closed account
        try {
            commandHandler->handleCommand(DepositMoneyCommand(account2Id, 50.0));
            std::cout << "✓ Deposit to closed account succeeded\n";
        } catch (const std::exception& e) {
            std::cout << "✗ Deposit to closed account failed: " << e.what() << "\n";
        }

        std::cout << "\n7. All events in chronological order:\n";
        auto allEvents = eventStorePtr->getAllEvents();
        for (size_t i = 0; i < allEvents.size(); ++i) {
            const auto& event = allEvents[i];
            std::cout << (i + 1) << ". " << event->getFormattedTimestamp()
                     << " - " << event->getEventType()
                     << " (Account: " << event->getAggregateId().toString() << ")\n";
        }

        std::cout << "\nTotal events stored: " << eventStorePtr->getEventCount() << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\n✅ Event Sourcing provides complete audit trail and allows\n";
    std::cout << "   reconstruction of state at any point in time!\n";

    return 0;
}