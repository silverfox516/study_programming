// Composition Over Inheritance
// Prefer composing behavior from parts rather than deep inheritance hierarchies.

#include <iostream>
#include <string>
#include <memory>
#include <vector>

// ============================================================
// BAD: Deep inheritance → rigid, combinatorial explosion
// ============================================================
namespace bad {

class Character {
public:
    virtual ~Character() = default;
    virtual void attack() { std::cout << "  Basic attack\n"; }
    virtual void move() { std::cout << "  Walk\n"; }
};

class FlyingCharacter : public Character {
public:
    void move() override { std::cout << "  Fly\n"; }
};

class SwimmingCharacter : public Character {
public:
    void move() override { std::cout << "  Swim\n"; }
};

// What about a character that can BOTH fly and swim?
// C++ has multiple inheritance, but it gets messy fast.
// class FlyingSwimmingCharacter : public FlyingCharacter, public SwimmingCharacter { ??? }

} // namespace bad

// ============================================================
// GOOD: Compose behavior from interchangeable parts
// ============================================================
namespace good {

// Movement strategy (composition)
class MovementStyle {
public:
    virtual ~MovementStyle() = default;
    virtual void move() = 0;
};

class Walk : public MovementStyle {
public:
    void move() override { std::cout << "  Walking\n"; }
};

class Fly : public MovementStyle {
public:
    void move() override { std::cout << "  Flying\n"; }
};

class Swim : public MovementStyle {
public:
    void move() override { std::cout << "  Swimming\n"; }
};

// Attack strategy (composition)
class AttackStyle {
public:
    virtual ~AttackStyle() = default;
    virtual void attack() = 0;
};

class Melee : public AttackStyle {
public:
    void attack() override { std::cout << "  Melee strike\n"; }
};

class Ranged : public AttackStyle {
public:
    void attack() override { std::cout << "  Ranged shot\n"; }
};

class Magic : public AttackStyle {
public:
    void attack() override { std::cout << "  Cast spell\n"; }
};

// Character composes behavior — no inheritance hierarchy
class Character {
    std::string name_;
    std::unique_ptr<MovementStyle> movement_;
    std::unique_ptr<AttackStyle> attack_;
public:
    Character(std::string name,
              std::unique_ptr<MovementStyle> mov,
              std::unique_ptr<AttackStyle> atk)
        : name_(std::move(name))
        , movement_(std::move(mov))
        , attack_(std::move(atk)) {}

    void perform() {
        std::cout << name_ << ":\n";
        movement_->move();
        attack_->attack();
    }

    // Can change behavior at runtime!
    void set_movement(std::unique_ptr<MovementStyle> m) { movement_ = std::move(m); }
};

} // namespace good

int main() {
    std::cout << "=== BAD: Inheritance hierarchy ===\n";
    {
        bad::Character warrior;
        warrior.attack();
        warrior.move();

        bad::FlyingCharacter dragon;
        dragon.attack();
        dragon.move();
        // Can't easily make a flying+swimming character
    }

    std::cout << "\n=== GOOD: Composition ===\n";
    {
        auto knight = good::Character("Knight",
            std::make_unique<good::Walk>(),
            std::make_unique<good::Melee>());
        knight.perform();

        auto dragon = good::Character("Dragon",
            std::make_unique<good::Fly>(),
            std::make_unique<good::Magic>());
        dragon.perform();

        // Flying + Swimming? Just compose differently!
        auto seabird = good::Character("Seabird",
            std::make_unique<good::Fly>(),
            std::make_unique<good::Ranged>());
        seabird.perform();

        // Change behavior at runtime
        std::cout << "\n-- Seabird lands in water --\n";
        seabird.set_movement(std::make_unique<good::Swim>());
        seabird.perform();
    }

    return 0;
}
