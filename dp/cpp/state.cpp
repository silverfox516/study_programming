#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <random>

// Forward declarations
class VendingMachine;
class HasMoneyState;
class DispensingState;
class IdleState;

// State interface
class VendingMachineState {
public:
    virtual ~VendingMachineState() = default;
    virtual void insertCoin(VendingMachine& machine, int coin) = 0;
    virtual void selectProduct(VendingMachine& machine, int productId) = 0;
    virtual void dispenseProduct(VendingMachine& machine) = 0;
    virtual void cancel(VendingMachine& machine) = 0;
    virtual std::string getStateName() const = 0;
};

// Context - Vending Machine
class VendingMachine {
private:
    std::unique_ptr<VendingMachineState> currentState;
    int balance;
    int selectedProduct;
    std::vector<std::pair<std::string, int>> products; // name, price
    std::vector<int> inventory; // stock count for each product

public:
    VendingMachine();

    template<typename T>
    void setState(std::unique_ptr<T> state) {
        currentState = std::move(state);
        std::cout << "State changed to: " << currentState->getStateName() << std::endl;
    }

    void insertCoin(int coin) {
        currentState->insertCoin(*this, coin);
    }

    void selectProduct(int productId) {
        currentState->selectProduct(*this, productId);
    }

    void dispenseProduct() {
        currentState->dispenseProduct(*this);
    }

    void cancel() {
        currentState->cancel(*this);
    }

    // Getters and setters
    int getBalance() const { return balance; }
    void addToBalance(int amount) { balance += amount; }
    void deductFromBalance(int amount) { balance -= amount; }
    void resetBalance() { balance = 0; }

    int getSelectedProduct() const { return selectedProduct; }
    void setSelectedProduct(int productId) { selectedProduct = productId; }

    const std::vector<std::pair<std::string, int>>& getProducts() const { return products; }
    const std::vector<int>& getInventory() const { return inventory; }

    bool hasStock(int productId) const {
        return productId >= 0 && productId < inventory.size() && inventory[productId] > 0;
    }

    void reduceInventory(int productId) {
        if (hasStock(productId)) {
            inventory[productId]--;
        }
    }

    void displayStatus() const {
        std::cout << "--- Vending Machine Status ---" << std::endl;
        std::cout << "Current State: " << currentState->getStateName() << std::endl;
        std::cout << "Balance: $" << balance / 100.0 << std::endl;
        std::cout << "Products:" << std::endl;
        for (size_t i = 0; i < products.size(); ++i) {
            std::cout << "  " << i << ": " << products[i].first
                      << " - $" << products[i].second / 100.0
                      << " (Stock: " << inventory[i] << ")" << std::endl;
        }
        std::cout << "----------------------------" << std::endl;
    }

    std::string getCurrentStateName() const {
        return currentState->getStateName();
    }
};

// Concrete States
class IdleState : public VendingMachineState {
public:
    void insertCoin(VendingMachine& machine, int coin) override {
        if (coin > 0) {
            machine.addToBalance(coin);
            std::cout << "Coin inserted: $" << coin / 100.0 << std::endl;
            machine.setState(std::make_unique<HasMoneyState>());
        } else {
            std::cout << "Invalid coin amount" << std::endl;
        }
    }

    void selectProduct(VendingMachine& machine, int productId) override {
        std::cout << "Please insert money first" << std::endl;
    }

    void dispenseProduct(VendingMachine& machine) override {
        std::cout << "No product selected and no money inserted" << std::endl;
    }

    void cancel(VendingMachine& machine) override {
        std::cout << "Nothing to cancel" << std::endl;
    }

    std::string getStateName() const override {
        return "Idle";
    }
};

class HasMoneyState : public VendingMachineState {
public:
    void insertCoin(VendingMachine& machine, int coin) override {
        if (coin > 0) {
            machine.addToBalance(coin);
            std::cout << "Additional coin inserted: $" << coin / 100.0 << std::endl;
            std::cout << "Total balance: $" << machine.getBalance() / 100.0 << std::endl;
        } else {
            std::cout << "Invalid coin amount" << std::endl;
        }
    }

    void selectProduct(VendingMachine& machine, int productId) override {
        const auto& products = machine.getProducts();

        if (productId < 0 || productId >= products.size()) {
            std::cout << "Invalid product selection" << std::endl;
            return;
        }

        if (!machine.hasStock(productId)) {
            std::cout << "Product out of stock: " << products[productId].first << std::endl;
            return;
        }

        int productPrice = products[productId].second;
        if (machine.getBalance() >= productPrice) {
            machine.setSelectedProduct(productId);
            std::cout << "Product selected: " << products[productId].first << std::endl;
            machine.setState(std::make_unique<DispensingState>());
        } else {
            std::cout << "Insufficient funds. Need $" << (productPrice - machine.getBalance()) / 100.0 << " more" << std::endl;
        }
    }

    void dispenseProduct(VendingMachine& machine) override {
        std::cout << "Please select a product first" << std::endl;
    }

    void cancel(VendingMachine& machine) override {
        int refund = machine.getBalance();
        machine.resetBalance();
        std::cout << "Transaction cancelled. Refund: $" << refund / 100.0 << std::endl;
        machine.setState(std::make_unique<IdleState>());
    }

    std::string getStateName() const override {
        return "Has Money";
    }
};

class DispensingState : public VendingMachineState {
public:
    void insertCoin(VendingMachine& machine, int coin) override {
        std::cout << "Please wait, dispensing product..." << std::endl;
    }

    void selectProduct(VendingMachine& machine, int productId) override {
        std::cout << "Already dispensing a product" << std::endl;
    }

    void dispenseProduct(VendingMachine& machine) override {
        int productId = machine.getSelectedProduct();
        const auto& products = machine.getProducts();

        std::cout << "Dispensing " << products[productId].first << "..." << std::endl;

        // Simulate dispensing time
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        machine.deductFromBalance(products[productId].second);
        machine.reduceInventory(productId);

        std::cout << "Product dispensed!" << std::endl;

        // Return change if any
        int change = machine.getBalance();
        if (change > 0) {
            std::cout << "Change returned: $" << change / 100.0 << std::endl;
            machine.resetBalance();
        }

        machine.setState(std::make_unique<IdleState>());
    }

    void cancel(VendingMachine& machine) override {
        std::cout << "Cannot cancel while dispensing" << std::endl;
    }

    std::string getStateName() const override {
        return "Dispensing";
    }
};

class OutOfOrderState : public VendingMachineState {
public:
    void insertCoin(VendingMachine& machine, int coin) override {
        std::cout << "Machine is out of order. Coin returned: $" << coin / 100.0 << std::endl;
    }

    void selectProduct(VendingMachine& machine, int productId) override {
        std::cout << "Machine is out of order" << std::endl;
    }

    void dispenseProduct(VendingMachine& machine) override {
        std::cout << "Machine is out of order" << std::endl;
    }

    void cancel(VendingMachine& machine) override {
        std::cout << "Machine is out of order" << std::endl;
    }

    std::string getStateName() const override {
        return "Out of Order";
    }
};

// Forward declarations for VendingMachine constructor
VendingMachine::VendingMachine() : balance(0), selectedProduct(-1) {
    // Initialize products (name, price in cents)
    products = {
        {"Coca Cola", 150},
        {"Pepsi", 150},
        {"Water", 100},
        {"Chips", 125},
        {"Candy", 75}
    };

    // Initialize inventory
    inventory = {5, 5, 10, 3, 8};

    // Start in idle state
    currentState = std::make_unique<IdleState>();
}

// Example 2: Traffic Light State Machine
class TrafficLight;
class GreenLightState;
class YellowLightState;

class TrafficLightState {
public:
    virtual ~TrafficLightState() = default;
    virtual void handle(TrafficLight& light) = 0;
    virtual std::string getColor() const = 0;
    virtual int getDuration() const = 0; // Duration in seconds
};

class TrafficLight {
private:
    std::unique_ptr<TrafficLightState> currentState;
    int timeRemaining;

public:
    TrafficLight();

    template<typename T>
    void setState(std::unique_ptr<T> state) {
        currentState = std::move(state);
        timeRemaining = currentState->getDuration();
        std::cout << "Traffic light changed to: " << currentState->getColor()
                  << " (" << timeRemaining << "s)" << std::endl;
    }

    void update() {
        if (timeRemaining > 0) {
            timeRemaining--;
            std::cout << "Traffic light: " << currentState->getColor()
                      << " (" << timeRemaining << "s remaining)" << std::endl;
        } else {
            currentState->handle(*this);
        }
    }

    std::string getCurrentColor() const {
        return currentState->getColor();
    }

    int getTimeRemaining() const {
        return timeRemaining;
    }
};

class RedLightState : public TrafficLightState {
public:
    void handle(TrafficLight& light) override {
        light.setState(std::make_unique<GreenLightState>());
    }

    std::string getColor() const override {
        return "RED";
    }

    int getDuration() const override {
        return 10; // 10 seconds
    }
};

class GreenLightState : public TrafficLightState {
public:
    void handle(TrafficLight& light) override {
        light.setState(std::make_unique<YellowLightState>());
    }

    std::string getColor() const override {
        return "GREEN";
    }

    int getDuration() const override {
        return 15; // 15 seconds
    }
};

class YellowLightState : public TrafficLightState {
public:
    void handle(TrafficLight& light) override {
        light.setState(std::make_unique<RedLightState>());
    }

    std::string getColor() const override {
        return "YELLOW";
    }

    int getDuration() const override {
        return 3; // 3 seconds
    }
};

TrafficLight::TrafficLight() : timeRemaining(0) {
    currentState = std::unique_ptr<TrafficLightState>(new RedLightState());
    timeRemaining = currentState->getDuration();
}

// Example 3: Game Character State Machine
class GameCharacter;
class MovingState;
class JumpingState;
class AttackingState;
class DeadState;
class IdleCharacterState;

class CharacterState {
public:
    virtual ~CharacterState() = default;
    virtual void handleInput(GameCharacter& character, const std::string& input) = 0;
    virtual void update(GameCharacter& character) = 0;
    virtual std::string getStateName() const = 0;
};

class GameCharacter {
private:
    std::unique_ptr<CharacterState> currentState;
    int health;
    int mana;
    bool isJumping;
    bool isAttacking;

public:
    GameCharacter();

    template<typename T>
    void setState(std::unique_ptr<T> state) {
        currentState = std::move(state);
        std::cout << "Character state: " << currentState->getStateName() << std::endl;
    }

    void handleInput(const std::string& input) {
        currentState->handleInput(*this, input);
    }

    void update() {
        currentState->update(*this);
    }

    // Getters and setters
    int getHealth() const { return health; }
    void setHealth(int h) { health = std::max(0, h); }

    int getMana() const { return mana; }
    void setMana(int m) { mana = std::max(0, std::min(100, m)); }

    bool getIsJumping() const { return isJumping; }
    void setIsJumping(bool jumping) { isJumping = jumping; }

    bool getIsAttacking() const { return isAttacking; }
    void setIsAttacking(bool attacking) { isAttacking = attacking; }

    void displayStatus() const {
        std::cout << "Character Status - State: " << currentState->getStateName()
                  << ", Health: " << health << ", Mana: " << mana
                  << ", Jumping: " << (isJumping ? "Yes" : "No")
                  << ", Attacking: " << (isAttacking ? "Yes" : "No") << std::endl;
    }

    std::string getCurrentStateName() const {
        return currentState->getStateName();
    }
};

class IdleCharacterState : public CharacterState {
public:
    void handleInput(GameCharacter& character, const std::string& input) override {
        if (input == "move") {
            character.setState(std::make_unique<MovingState>());
        } else if (input == "jump") {
            character.setState(std::make_unique<JumpingState>());
        } else if (input == "attack") {
            character.setState(std::make_unique<AttackingState>());
        } else if (input == "die") {
            character.setHealth(0);
            character.setState(std::make_unique<DeadState>());
        }
    }

    void update(GameCharacter& character) override {
        // Regenerate mana while idle
        character.setMana(character.getMana() + 1);
    }

    std::string getStateName() const override {
        return "Idle";
    }
};

class MovingState : public CharacterState {
private:
    int moveDuration = 3;

public:
    void handleInput(GameCharacter& character, const std::string& input) override {
        if (input == "stop") {
            character.setState(std::make_unique<IdleCharacterState>());
        } else if (input == "jump") {
            character.setState(std::make_unique<JumpingState>());
        } else if (input == "attack") {
            character.setState(std::make_unique<AttackingState>());
        }
    }

    void update(GameCharacter& character) override {
        moveDuration--;
        std::cout << "Moving... (" << moveDuration << " seconds remaining)" << std::endl;
        if (moveDuration <= 0) {
            character.setState(std::make_unique<IdleCharacterState>());
        }
    }

    std::string getStateName() const override {
        return "Moving";
    }
};

class JumpingState : public CharacterState {
private:
    int jumpDuration = 2;

public:
    void handleInput(GameCharacter& character, const std::string& input) override {
        if (input == "attack") {
            character.setState(std::make_unique<AttackingState>());
        }
        // Ignore other inputs while jumping
    }

    void update(GameCharacter& character) override {
        character.setIsJumping(true);
        jumpDuration--;
        std::cout << "Jumping... (" << jumpDuration << " seconds remaining)" << std::endl;
        if (jumpDuration <= 0) {
            character.setIsJumping(false);
            character.setState(std::make_unique<IdleCharacterState>());
        }
    }

    std::string getStateName() const override {
        return "Jumping";
    }
};

class AttackingState : public CharacterState {
private:
    int attackDuration = 2;

public:
    void handleInput(GameCharacter& character, const std::string& input) override {
        // Cannot interrupt attack
    }

    void update(GameCharacter& character) override {
        character.setIsAttacking(true);
        character.setMana(character.getMana() - 5); // Use mana for attack

        attackDuration--;
        std::cout << "Attacking... (" << attackDuration << " seconds remaining)" << std::endl;

        if (attackDuration <= 0) {
            character.setIsAttacking(false);
            character.setState(std::make_unique<IdleCharacterState>());
        }
    }

    std::string getStateName() const override {
        return "Attacking";
    }
};

class DeadState : public CharacterState {
public:
    void handleInput(GameCharacter& character, const std::string& input) override {
        if (input == "respawn") {
            character.setHealth(100);
            character.setMana(100);
            character.setState(std::make_unique<IdleCharacterState>());
            std::cout << "Character respawned!" << std::endl;
        }
    }

    void update(GameCharacter& character) override {
        // Dead characters don't update
    }

    std::string getStateName() const override {
        return "Dead";
    }
};

GameCharacter::GameCharacter() : health(100), mana(100), isJumping(false), isAttacking(false) {
    currentState = std::make_unique<IdleCharacterState>();
}

int main() {
    std::cout << "=== State Pattern Demo ===" << std::endl;

    // Example 1: Vending Machine
    std::cout << "\n1. Vending Machine State Machine:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    VendingMachine machine;
    machine.displayStatus();

    std::cout << "\nTesting vending machine operations:" << std::endl;

    // Try to select product without money
    machine.selectProduct(0);

    // Insert money
    machine.insertCoin(100); // $1.00
    machine.selectProduct(0); // Try to buy $1.50 product

    // Insert more money
    machine.insertCoin(75); // $0.75 more
    machine.selectProduct(0); // Should work now
    machine.dispenseProduct();

    machine.displayStatus();

    // Test cancellation
    std::cout << "\nTesting cancellation:" << std::endl;
    machine.insertCoin(200);
    machine.cancel();

    // Example 2: Traffic Light
    std::cout << "\n\n2. Traffic Light State Machine:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    TrafficLight trafficLight;

    std::cout << "Simulating traffic light for 30 seconds:" << std::endl;
    for (int i = 0; i < 30; ++i) {
        trafficLight.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Speed up simulation
    }

    // Example 3: Game Character
    std::cout << "\n\n3. Game Character State Machine:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    GameCharacter character;
    character.displayStatus();

    std::cout << "\nSimulating character actions:" << std::endl;

    // Character starts idle, move around
    character.handleInput("move");
    character.displayStatus();

    for (int i = 0; i < 3; ++i) {
        character.update();
        character.displayStatus();
    }

    // Jump while moving
    character.handleInput("jump");
    character.displayStatus();

    for (int i = 0; i < 2; ++i) {
        character.update();
        character.displayStatus();
    }

    // Attack
    character.handleInput("attack");
    character.displayStatus();

    for (int i = 0; i < 2; ++i) {
        character.update();
        character.displayStatus();
    }

    // Kill character and respawn
    std::cout << "\nKilling character and respawning:" << std::endl;
    character.handleInput("die");
    character.displayStatus();

    character.handleInput("respawn");
    character.displayStatus();

    // Test state transitions with complex scenario
    std::cout << "\n\n4. Complex State Transition Scenario:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    VendingMachine machine2;

    // Simulate a full transaction
    std::cout << "Customer inserts $2.00:" << std::endl;
    machine2.insertCoin(200);

    std::cout << "\nCustomer selects product 1 (Pepsi - $1.50):" << std::endl;
    machine2.selectProduct(1);

    std::cout << "\nMachine dispenses product:" << std::endl;
    machine2.dispenseProduct();

    machine2.displayStatus();

    // State Pattern Benefits
    std::cout << "\n\n5. State Pattern Benefits:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "✓ Localizes state-specific behavior in separate classes" << std::endl;
    std::cout << "✓ Makes state transitions explicit" << std::endl;
    std::cout << "✓ Eliminates large conditional statements" << std::endl;
    std::cout << "✓ Makes it easy to add new states" << std::endl;
    std::cout << "✓ Each state can maintain its own data" << std::endl;
    std::cout << "✓ Supports polymorphic behavior based on state" << std::endl;
    std::cout << "✓ Makes state machines easier to understand and maintain" << std::endl;

    return 0;
}