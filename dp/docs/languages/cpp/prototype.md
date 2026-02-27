# Prototype Pattern - C++ Implementation

## 개요

Prototype 패턴은 기존 객체를 복사하여 새 객체를 생성하는 생성 패턴입니다. 클래스를 직접 인스턴스화하지 않고 기존 객체를 클론하여 새로운 객체를 만들므로, 복잡한 객체의 생성 비용을 줄이고 런타임에 객체 타입을 결정할 수 있습니다.

## 구조

- **Prototype**: 클론 메서드를 선언하는 인터페이스
- **ConcretePrototype**: 클론 메서드를 구현하는 구체 클래스
- **Client**: Prototype을 복사해 새 객체를 생성

## C++ 구현

### 1. 기본 Prototype 인터페이스

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Prototype {
public:
    virtual ~Prototype() = default;
    virtual std::unique_ptr<Prototype> clone() const = 0;
    virtual void display() const = 0;
    virtual std::string getType() const = 0;
};
```

### 2. Concrete Prototype - Enemy 클래스

```cpp
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

    std::string getType() const override { return "Enemy"; }

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
};
```

### 3. Prototype Registry

```cpp
class PrototypeRegistry {
private:
    std::unordered_map<std::string, std::unique_ptr<Prototype>> prototypes;

public:
    void registerPrototype(const std::string& key, std::unique_ptr<Prototype> prototype) {
        prototypes[key] = std::move(prototype);
    }

    std::unique_ptr<Prototype> createObject(const std::string& key) const {
        auto it = prototypes.find(key);
        if (it != prototypes.end()) {
            return it->second->clone();
        }
        throw std::runtime_error("Prototype not found: " + key);
    }

    bool hasPrototype(const std::string& key) const {
        return prototypes.find(key) != prototypes.end();
    }

    std::vector<std::string> getAvailableTypes() const {
        std::vector<std::string> types;
        for (const auto& [key, _] : prototypes) {
            types.push_back(key);
        }
        return types;
    }
};
```

## 사용 예제

```cpp
int main() {
    // Prototype Registry 생성
    PrototypeRegistry registry;

    // 기본 적 프로토타입들 등록
    auto goblin = std::make_unique<Enemy>("Goblin", 50, 15, 5);
    goblin->addAbility("Quick Strike");

    auto orc = std::make_unique<Enemy>("Orc", 100, 25, 10);
    orc->addAbility("Rage");
    orc->addAbility("Intimidate");

    registry.registerPrototype("goblin", std::move(goblin));
    registry.registerPrototype("orc", std::move(orc));

    // 프로토타입으로부터 새 객체들 생성
    auto goblin1 = registry.createObject("goblin");
    auto goblin2 = registry.createObject("goblin");

    // 각각 다르게 수정
    static_cast<Enemy*>(goblin1.get())->setName("Goblin Warrior");
    static_cast<Enemy*>(goblin1.get())->modifyStats(20, 5, 2);

    static_cast<Enemy*>(goblin2.get())->setName("Goblin Scout");
    static_cast<Enemy*>(goblin2.get())->addAbility("Stealth");

    // 결과 출력
    goblin1->display();
    goblin2->display();

    return 0;
}
```

## C++의 장점

1. **복사 생성자**: C++의 복사 생성자를 활용한 자연스러운 클로닝
2. **스마트 포인터**: `unique_ptr`로 안전한 메모리 관리
3. **타입 안전성**: 컴파일 타임 타입 체크
4. **성능**: 직접 메모리 조작으로 효율적인 복사

## 적용 상황

1. **게임 개발**: 몬스터, 아이템 생성
2. **GUI 개발**: 위젯 복사
3. **문서 편집기**: 객체 복사/붙여넣기
4. **설정 관리**: 기본 설정 복사
5. **테스트**: 테스트 데이터 생성

Prototype 패턴은 복잡한 객체 생성을 단순화하고 런타임 유연성을 제공하는 강력한 패턴입니다.