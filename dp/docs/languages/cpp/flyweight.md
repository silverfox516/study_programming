# Flyweight Pattern - C++ Implementation

## 개요

Flyweight 패턴은 많은 수의 유사한 객체를 효율적으로 지원하기 위한 구조적 디자인 패턴입니다. 공통된 상태(내재적 상태)를 공유하여 메모리 사용량을 최소화하고, 각 객체의 고유한 상태(외재적 상태)는 클라이언트에서 관리합니다. 특히 대량의 객체가 필요한 상황에서 메모리 효율성을 크게 향상시킬 수 있습니다.

## 구조

Flyweight 패턴의 주요 구성 요소:
- **Flyweight**: 내재적 상태를 저장하고 외재적 상태를 매개변수로 받는 인터페이스
- **ConcreteFlyweight**: 내재적 상태를 실제로 저장하는 구현 클래스
- **FlyweightFactory**: Flyweight 객체를 생성하고 관리하는 팩토리
- **Context**: 외재적 상태를 저장하고 Flyweight를 참조하는 클래스

## C++ 구현

### 1. Flyweight 인터페이스와 구현

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Flyweight interface
class CharacterFlyweight {
public:
    virtual ~CharacterFlyweight() = default;
    virtual void render(int x, int y, int size, const std::string& color) const = 0;
    virtual char getCharacter() const = 0;
    virtual std::string getFontFamily() const = 0;
};

// Concrete Flyweight
class ConcreteCharacterFlyweight : public CharacterFlyweight {
private:
    char character;      // Intrinsic state (내재적 상태)
    std::string fontFamily;  // Intrinsic state

public:
    ConcreteCharacterFlyweight(char character, const std::string& fontFamily)
        : character(character), fontFamily(fontFamily) {}

    void render(int x, int y, int size, const std::string& color) const override {
        std::cout << "Rendering '" << character << "' at (" << x << "," << y
                  << ") size=" << size << " color=" << color
                  << " font=" << fontFamily << std::endl;
    }

    char getCharacter() const override {
        return character;
    }

    std::string getFontFamily() const override {
        return fontFamily;
    }
};
```

### 2. Flyweight Factory

```cpp
class CharacterFlyweightFactory {
private:
    std::unordered_map<std::string, std::shared_ptr<CharacterFlyweight>> flyweights;

    std::string getKey(char character, const std::string& fontFamily) const {
        return std::string(1, character) + "_" + fontFamily;
    }

public:
    std::shared_ptr<CharacterFlyweight> getFlyweight(char character, const std::string& fontFamily) {
        std::string key = getKey(character, fontFamily);

        auto it = flyweights.find(key);
        if (it != flyweights.end()) {
            std::cout << "Reusing existing flyweight for '" << character
                     << "' (" << fontFamily << ")" << std::endl;
            return it->second;
        }

        std::cout << "Creating new flyweight for '" << character
                 << "' (" << fontFamily << ")" << std::endl;
        auto flyweight = std::make_shared<ConcreteCharacterFlyweight>(character, fontFamily);
        flyweights[key] = flyweight;
        return flyweight;
    }

    size_t getFlyweightCount() const {
        return flyweights.size();
    }

    void listFlyweights() const {
        std::cout << "Total flyweights created: " << flyweights.size() << std::endl;
        for (const auto& [key, flyweight] : flyweights) {
            std::cout << "  " << key << std::endl;
        }
    }
};
```

### 3. Context 클래스

```cpp
// Context class that uses flyweights
class TextCharacter {
private:
    std::shared_ptr<CharacterFlyweight> flyweight;
    int x, y;           // Extrinsic state (외재적 상태)
    int size;           // Extrinsic state
    std::string color;  // Extrinsic state

public:
    TextCharacter(std::shared_ptr<CharacterFlyweight> flyweight,
                  int x, int y, int size, const std::string& color)
        : flyweight(flyweight), x(x), y(y), size(size), color(color) {}

    void render() const {
        flyweight->render(x, y, size, color);
    }

    void move(int newX, int newY) {
        x = newX;
        y = newY;
    }

    void changeSize(int newSize) {
        size = newSize;
    }

    void changeColor(const std::string& newColor) {
        color = newColor;
    }

    // Getters
    char getCharacter() const { return flyweight->getCharacter(); }
    std::string getFontFamily() const { return flyweight->getFontFamily(); }
    int getX() const { return x; }
    int getY() const { return y; }
    int getSize() const { return size; }
    std::string getColor() const { return color; }
};
```

### 4. 텍스트 문서 클래스

```cpp
class TextDocument {
private:
    std::vector<std::unique_ptr<TextCharacter>> characters;
    CharacterFlyweightFactory& factory;

public:
    explicit TextDocument(CharacterFlyweightFactory& factory) : factory(factory) {}

    void addCharacter(char c, const std::string& font, int x, int y,
                     int size, const std::string& color) {
        auto flyweight = factory.getFlyweight(c, font);
        characters.push_back(std::make_unique<TextCharacter>(flyweight, x, y, size, color));
    }

    void render() const {
        std::cout << "\\nRendering document with " << characters.size() << " characters:" << std::endl;
        for (const auto& character : characters) {
            character->render();
        }
    }

    void renderRange(size_t start, size_t count) const {
        size_t end = std::min(start + count, characters.size());
        std::cout << "\\nRendering characters " << start << " to " << (end-1) << ":" << std::endl;

        for (size_t i = start; i < end; ++i) {
            characters[i]->render();
        }
    }

    size_t getCharacterCount() const {
        return characters.size();
    }

    void moveCharacter(size_t index, int newX, int newY) {
        if (index < characters.size()) {
            characters[index]->move(newX, newY);
        }
    }

    void changeCharacterColor(size_t index, const std::string& newColor) {
        if (index < characters.size()) {
            characters[index]->changeColor(newColor);
        }
    }
};
```

### 5. 게임 객체 예제

```cpp
// 게임에서의 Flyweight 패턴 사용 예제
class TreeType {
private:
    std::string name;        // Intrinsic state
    std::string sprite;      // Intrinsic state
    std::string color;       // Intrinsic state

public:
    TreeType(const std::string& name, const std::string& sprite, const std::string& color)
        : name(name), sprite(sprite), color(color) {}

    void render(double x, double y, double size) const {
        std::cout << "Drawing " << name << " tree (sprite: " << sprite
                  << ", color: " << color << ") at (" << x << ", " << y
                  << ") with size " << size << std::endl;
    }

    std::string getName() const { return name; }
};

class TreeTypeFactory {
private:
    static std::unordered_map<std::string, std::shared_ptr<TreeType>> treeTypes;

public:
    static std::shared_ptr<TreeType> getTreeType(const std::string& name,
                                                const std::string& sprite,
                                                const std::string& color) {
        std::string key = name + "_" + sprite + "_" + color;

        auto it = treeTypes.find(key);
        if (it != treeTypes.end()) {
            return it->second;
        }

        auto treeType = std::make_shared<TreeType>(name, sprite, color);
        treeTypes[key] = treeType;
        return treeType;
    }

    static size_t getCreatedTreeTypesCount() {
        return treeTypes.size();
    }
};

// Static member definition
std::unordered_map<std::string, std::shared_ptr<TreeType>> TreeTypeFactory::treeTypes;

class Tree {
private:
    double x, y;        // Extrinsic state
    double size;        // Extrinsic state
    std::shared_ptr<TreeType> treeType;

public:
    Tree(double x, double y, double size, std::shared_ptr<TreeType> treeType)
        : x(x), y(y), size(size), treeType(treeType) {}

    void render() const {
        treeType->render(x, y, size);
    }

    void move(double newX, double newY) {
        x = newX;
        y = newY;
    }

    void changeSize(double newSize) {
        size = newSize;
    }
};
```

## 사용 예제

```cpp
int main() {
    // 1. 텍스트 에디터 예제
    CharacterFlyweightFactory factory;
    TextDocument document(factory);

    // 문서에 텍스트 추가
    std::string text = "Hello World! Hello C++!";
    int x = 0, y = 0;

    for (char c : text) {
        document.addCharacter(c, "Arial", x, y, 12, "black");
        x += 10; // 다음 문자 위치
    }

    // 일부 문자들을 다른 폰트로 추가
    for (char c : "BOLD") {
        document.addCharacter(c, "Arial Bold", x, y, 14, "red");
        x += 12;
    }

    document.render();

    std::cout << "\\nTotal characters in document: " << document.getCharacterCount() << std::endl;
    factory.listFlyweights();

    // 2. 게임 세계의 나무들
    std::vector<std::unique_ptr<Tree>> forest;

    // 1000개의 나무 생성 (하지만 실제로는 적은 수의 TreeType만 생성됨)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> pos_dist(0.0, 1000.0);
    std::uniform_real_distribution<> size_dist(1.0, 5.0);
    std::uniform_int_distribution<> type_dist(0, 2);

    std::vector<std::string> treeNames = {"Oak", "Pine", "Birch"};
    std::vector<std::string> sprites = {"oak_sprite", "pine_sprite", "birch_sprite"};
    std::vector<std::string> colors = {"green", "dark_green", "light_green"};

    for (int i = 0; i < 1000; ++i) {
        int typeIndex = type_dist(gen);
        auto treeType = TreeTypeFactory::getTreeType(
            treeNames[typeIndex],
            sprites[typeIndex],
            colors[typeIndex]
        );

        forest.push_back(std::make_unique<Tree>(
            pos_dist(gen), pos_dist(gen), size_dist(gen), treeType
        ));
    }

    std::cout << "\\n=== Forest Simulation ===" << std::endl;
    std::cout << "Created " << forest.size() << " trees" << std::endl;
    std::cout << "But only " << TreeTypeFactory::getCreatedTreeTypesCount()
              << " TreeType flyweights were created!" << std::endl;

    // 처음 5개 나무만 렌더링
    std::cout << "\\nRendering first 5 trees:" << std::endl;
    for (size_t i = 0; i < 5 && i < forest.size(); ++i) {
        forest[i]->render();
    }

    return 0;
}
```

## C++의 장점

1. **스마트 포인터 활용**: `shared_ptr`로 안전한 메모리 관리
2. **STL 컨테이너**: `unordered_map`으로 효율적인 Flyweight 검색
3. **RAII**: 자동 리소스 관리
4. **타입 안전성**: 컴파일 타임 타입 검증
5. **성능**: 메모리 접근 최적화

## 적용 상황

1. **텍스트 에디터**: 문자 렌더링 최적화
2. **게임 엔진**: 스프라이트, 텍스처, 파티클 시스템
3. **GUI 시스템**: 아이콘, 위젯 스타일
4. **웹 브라우저**: DOM 요소 스타일링
5. **CAD 시스템**: 반복되는 도형 요소

### 메모리 효율성

```cpp
// Flyweight 없이: 1000개 객체 = 1000 * sizeof(TreeData)
// Flyweight 사용: 3개 TreeType + 1000개 위치 정보 = 훨씬 적은 메모리
```

Flyweight 패턴은 대량의 유사한 객체가 필요한 상황에서 메모리 사용량을 크게 줄이면서도 개별 객체의 유연성을 유지할 수 있는 강력한 패턴입니다.