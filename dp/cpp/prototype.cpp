#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <random>

// Abstract Prototype
class Prototype {
public:
    virtual ~Prototype() = default;
    virtual std::unique_ptr<Prototype> clone() const = 0;
    virtual void display() const = 0;
    virtual std::string getType() const = 0;
};

// Concrete Prototype - Enemy
class Enemy : public Prototype {
protected:
    std::string name;
    int health;
    int attack;
    int defense;
    std::vector<std::string> abilities;

public:
    Enemy(const std::string& name, int health, int attack, int defense)
        : name(name), health(health), attack(attack), defense(defense) {}

    // Copy constructor for deep copying
    Enemy(const Enemy& other)
        : name(other.name), health(other.health), attack(other.attack),
          defense(other.defense), abilities(other.abilities) {}

    std::unique_ptr<Prototype> clone() const override {
        return std::make_unique<Enemy>(*this);
    }

    void display() const override {
        std::cout << "Enemy: " << name << " (HP: " << health
                  << ", ATK: " << attack << ", DEF: " << defense << ")";
        if (!abilities.empty()) {
            std::cout << " - Abilities: ";
            for (size_t i = 0; i < abilities.size(); ++i) {
                std::cout << abilities[i];
                if (i < abilities.size() - 1) std::cout << ", ";
            }
        }
        std::cout << std::endl;
    }

    std::string getType() const override {
        return "Enemy";
    }

    void addAbility(const std::string& ability) {
        abilities.push_back(ability);
    }

    void modifyStats(int healthMod, int attackMod, int defenseMod) {
        health += healthMod;
        attack += attackMod;
        defense += defenseMod;
    }

    void setName(const std::string& newName) {
        name = newName;
    }

    // Getters
    int getHealth() const { return health; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    const std::string& getName() const { return name; }
};

// Concrete Prototype - Weapon
class Weapon : public Prototype {
private:
    std::string name;
    int damage;
    std::string type;
    std::vector<std::string> enchantments;
    bool isUpgraded;

public:
    Weapon(const std::string& name, int damage, const std::string& type)
        : name(name), damage(damage), type(type), isUpgraded(false) {}

    // Copy constructor
    Weapon(const Weapon& other)
        : name(other.name), damage(other.damage), type(other.type),
          enchantments(other.enchantments), isUpgraded(other.isUpgraded) {}

    std::unique_ptr<Prototype> clone() const override {
        return std::make_unique<Weapon>(*this);
    }

    void display() const override {
        std::cout << "Weapon: " << name << " (" << type << ") - Damage: " << damage;
        if (isUpgraded) {
            std::cout << " [UPGRADED]";
        }
        if (!enchantments.empty()) {
            std::cout << " - Enchantments: ";
            for (size_t i = 0; i < enchantments.size(); ++i) {
                std::cout << enchantments[i];
                if (i < enchantments.size() - 1) std::cout << ", ";
            }
        }
        std::cout << std::endl;
    }

    std::string getType() const override {
        return "Weapon";
    }

    void addEnchantment(const std::string& enchantment) {
        enchantments.push_back(enchantment);
    }

    void upgrade() {
        isUpgraded = true;
        damage = static_cast<int>(damage * 1.5);
    }

    void setName(const std::string& newName) {
        name = newName;
    }

    int getDamage() const { return damage; }
    const std::string& getName() const { return name; }
};

// Concrete Prototype - Character
class Character : public Prototype {
private:
    std::string name;
    std::string characterClass;
    int level;
    std::unique_ptr<Weapon> weapon;
    std::vector<std::string> skills;

public:
    Character(const std::string& name, const std::string& characterClass, int level)
        : name(name), characterClass(characterClass), level(level) {}

    // Copy constructor with deep copy of weapon
    Character(const Character& other)
        : name(other.name), characterClass(other.characterClass),
          level(other.level), skills(other.skills) {
        if (other.weapon) {
            weapon = std::unique_ptr<Weapon>(
                static_cast<Weapon*>(other.weapon->clone().release())
            );
        }
    }

    std::unique_ptr<Prototype> clone() const override {
        return std::make_unique<Character>(*this);
    }

    void display() const override {
        std::cout << "Character: " << name << " (Class: " << characterClass
                  << ", Level: " << level << ")";
        if (weapon) {
            std::cout << "\n  Equipped: ";
            weapon->display();
        }
        if (!skills.empty()) {
            std::cout << "  Skills: ";
            for (size_t i = 0; i < skills.size(); ++i) {
                std::cout << skills[i];
                if (i < skills.size() - 1) std::cout << ", ";
            }
            std::cout << std::endl;
        }
    }

    std::string getType() const override {
        return "Character";
    }

    void equipWeapon(std::unique_ptr<Weapon> newWeapon) {
        weapon = std::move(newWeapon);
    }

    void addSkill(const std::string& skill) {
        skills.push_back(skill);
    }

    void levelUp() {
        level++;
    }

    void setName(const std::string& newName) {
        name = newName;
    }

    int getLevel() const { return level; }
    const std::string& getName() const { return name; }
};

// Prototype Registry
class PrototypeRegistry {
private:
    std::unordered_map<std::string, std::unique_ptr<Prototype>> prototypes;

public:
    void registerPrototype(const std::string& key, std::unique_ptr<Prototype> prototype) {
        prototypes[key] = std::move(prototype);
    }

    std::unique_ptr<Prototype> create(const std::string& key) {
        auto it = prototypes.find(key);
        if (it != prototypes.end()) {
            return it->second->clone();
        }
        return nullptr;
    }

    void listPrototypes() const {
        std::cout << "Registered prototypes:" << std::endl;
        for (const auto& [key, prototype] : prototypes) {
            std::cout << "  " << key << " (" << prototype->getType() << ")" << std::endl;
        }
    }
};

// Factory using prototypes
class GameObjectFactory {
private:
    PrototypeRegistry registry;
    std::mt19937 rng;

public:
    GameObjectFactory() : rng(std::random_device{}()) {
        initializePrototypes();
    }

    void initializePrototypes() {
        // Register enemy prototypes
        auto goblin = std::make_unique<Enemy>("Goblin", 30, 8, 3);
        goblin->addAbility("Stealth");
        registry.registerPrototype("goblin", std::move(goblin));

        auto orc = std::make_unique<Enemy>("Orc", 80, 15, 8);
        orc->addAbility("Rage");
        orc->addAbility("Intimidate");
        registry.registerPrototype("orc", std::move(orc));

        auto dragon = std::make_unique<Enemy>("Dragon", 500, 50, 25);
        dragon->addAbility("Fire Breath");
        dragon->addAbility("Flight");
        dragon->addAbility("Magic Resistance");
        registry.registerPrototype("dragon", std::move(dragon));

        // Register weapon prototypes
        auto sword = std::make_unique<Weapon>("Iron Sword", 20, "Sword");
        registry.registerPrototype("iron_sword", std::move(sword));

        auto bow = std::make_unique<Weapon>("Elven Bow", 18, "Bow");
        bow->addEnchantment("Precision");
        registry.registerPrototype("elven_bow", std::move(bow));

        auto staff = std::make_unique<Weapon>("Magic Staff", 25, "Staff");
        staff->addEnchantment("Mana Boost");
        staff->addEnchantment("Spell Power");
        registry.registerPrototype("magic_staff", std::move(staff));

        // Register character prototypes
        auto warrior = std::make_unique<Character>("Template Warrior", "Warrior", 1);
        warrior->addSkill("Sword Mastery");
        warrior->addSkill("Shield Block");
        registry.registerPrototype("warrior", std::move(warrior));

        auto mage = std::make_unique<Character>("Template Mage", "Mage", 1);
        mage->addSkill("Fireball");
        mage->addSkill("Magic Shield");
        registry.registerPrototype("mage", std::move(mage));
    }

    std::unique_ptr<Prototype> createEnemy(const std::string& type) {
        auto enemy = registry.create(type);
        if (enemy && enemy->getType() == "Enemy") {
            // Add some random variation
            auto* enemyPtr = static_cast<Enemy*>(enemy.get());
            std::uniform_int_distribution<int> statVariation(-5, 5);
            enemyPtr->modifyStats(
                statVariation(rng),
                statVariation(rng),
                statVariation(rng)
            );
        }
        return enemy;
    }

    std::unique_ptr<Prototype> createWeapon(const std::string& type) {
        return registry.create(type);
    }

    std::unique_ptr<Prototype> createCharacter(const std::string& type, const std::string& name) {
        auto character = registry.create(type);
        if (character && character->getType() == "Character") {
            static_cast<Character*>(character.get())->setName(name);
        }
        return character;
    }

    void listAvailableTypes() const {
        registry.listPrototypes();
    }
};

int main() {
    std::cout << "=== Prototype Pattern Demo ===" << std::endl;

    GameObjectFactory factory;

    std::cout << "\nAvailable prototypes:" << std::endl;
    factory.listAvailableTypes();

    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "Creating enemies using prototypes:" << std::endl;

    // Create multiple goblins with variations
    std::vector<std::unique_ptr<Prototype>> enemies;
    for (int i = 1; i <= 3; ++i) {
        auto goblin = factory.createEnemy("goblin");
        if (goblin) {
            static_cast<Enemy*>(goblin.get())->setName("Goblin #" + std::to_string(i));
            enemies.push_back(std::move(goblin));
        }
    }

    // Create orcs
    for (int i = 1; i <= 2; ++i) {
        auto orc = factory.createEnemy("orc");
        if (orc) {
            static_cast<Enemy*>(orc.get())->setName("Orc Warrior #" + std::to_string(i));
            enemies.push_back(std::move(orc));
        }
    }

    // Create a dragon
    auto dragon = factory.createEnemy("dragon");
    if (dragon) {
        static_cast<Enemy*>(dragon.get())->setName("Ancient Red Dragon");
        enemies.push_back(std::move(dragon));
    }

    std::cout << "\nGenerated enemies:" << std::endl;
    for (const auto& enemy : enemies) {
        enemy->display();
    }

    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "Creating weapons using prototypes:" << std::endl;

    auto sword1 = factory.createWeapon("iron_sword");
    auto sword2 = factory.createWeapon("iron_sword");

    if (sword1 && sword2) {
        static_cast<Weapon*>(sword1.get())->setName("Knight's Sword");
        static_cast<Weapon*>(sword2.get())->setName("Rusty Sword");
        static_cast<Weapon*>(sword2.get())->addEnchantment("Poison");

        std::cout << "Original and modified swords:" << std::endl;
        sword1->display();
        sword2->display();
    }

    auto staff = factory.createWeapon("magic_staff");
    if (staff) {
        static_cast<Weapon*>(staff.get())->upgrade();
        static_cast<Weapon*>(staff.get())->setName("Archmage's Staff");
        std::cout << "\nUpgraded staff:" << std::endl;
        staff->display();
    }

    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "Creating characters using prototypes:" << std::endl;

    auto warrior = factory.createCharacter("warrior", "Sir Galahad");
    auto mage = factory.createCharacter("mage", "Merlin");

    if (warrior) {
        auto* warriorPtr = static_cast<Character*>(warrior.get());
        auto weaponCopy = factory.createWeapon("iron_sword");
        if (weaponCopy) {
            static_cast<Weapon*>(weaponCopy.get())->setName("Excalibur");
            static_cast<Weapon*>(weaponCopy.get())->addEnchantment("Holy Strike");
            warriorPtr->equipWeapon(std::unique_ptr<Weapon>(
                static_cast<Weapon*>(weaponCopy.release())
            ));
        }
        warriorPtr->levelUp();
        warriorPtr->addSkill("Battle Cry");

        std::cout << "\nCustomized warrior:" << std::endl;
        warrior->display();
    }

    if (mage) {
        auto* magePtr = static_cast<Character*>(mage.get());
        auto staffCopy = factory.createWeapon("magic_staff");
        if (staffCopy) {
            static_cast<Weapon*>(staffCopy.get())->setName("Staff of Wisdom");
            magePtr->equipWeapon(std::unique_ptr<Weapon>(
                static_cast<Weapon*>(staffCopy.release())
            ));
        }
        magePtr->levelUp();
        magePtr->levelUp();
        magePtr->addSkill("Teleport");
        magePtr->addSkill("Lightning Bolt");

        std::cout << "\nCustomized mage:" << std::endl;
        mage->display();
    }

    return 0;
}