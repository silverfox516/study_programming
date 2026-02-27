# Decorator Pattern - C++ Implementation

## 개요

Decorator 패턴은 객체에 추가적인 기능을 동적으로 첨부할 수 있게 해주는 구조적 패턴입니다. 이 패턴은 상속 대신 객체 컴포지션을 사용하여 기존 객체의 기능을 확장하며, 런타임에 객체의 행위를 변경할 수 있게 해줍니다.

## 구조

Decorator 패턴의 주요 구성 요소:
- **Component**: 기본 인터페이스 정의
- **ConcreteComponent**: 기본 구현 클래스
- **Decorator**: 컴포넌트 인터페이스를 구현하고 컴포넌트 참조를 가짐
- **ConcreteDecorator**: 구체적인 데코레이터 구현

## C++ 구현

### 1. 커피 주문 시스템

```cpp
#include <iostream>
#include <memory>
#include <string>

// Component interface
class Coffee {
public:
    virtual ~Coffee() = default;
    virtual std::string getDescription() const = 0;
    virtual double getCost() const = 0;
    virtual std::string getSize() const { return "Regular"; }
};

// Concrete Component
class BasicCoffee : public Coffee {
private:
    std::string size;

public:
    explicit BasicCoffee(const std::string& size = "Regular") : size(size) {}

    std::string getDescription() const override {
        return size + " Coffee";
    }

    double getCost() const override {
        if (size == "Small") return 2.50;
        if (size == "Large") return 4.50;
        return 3.50; // Regular
    }

    std::string getSize() const override {
        return size;
    }
};

// Base Decorator
class CoffeeDecorator : public Coffee {
protected:
    std::unique_ptr<Coffee> coffee;

public:
    explicit CoffeeDecorator(std::unique_ptr<Coffee> coffee)
        : coffee(std::move(coffee)) {}

    std::string getDescription() const override {
        return coffee->getDescription();
    }

    double getCost() const override {
        return coffee->getCost();
    }

    std::string getSize() const override {
        return coffee->getSize();
    }
};

// Concrete Decorators
class MilkDecorator : public CoffeeDecorator {
public:
    explicit MilkDecorator(std::unique_ptr<Coffee> coffee)
        : CoffeeDecorator(std::move(coffee)) {}

    std::string getDescription() const override {
        return coffee->getDescription() + " + Milk";
    }

    double getCost() const override {
        return coffee->getCost() + 0.60;
    }
};

class VanillaDecorator : public CoffeeDecorator {
public:
    explicit VanillaDecorator(std::unique_ptr<Coffee> coffee)
        : CoffeeDecorator(std::move(coffee)) {}

    std::string getDescription() const override {
        return coffee->getDescription() + " + Vanilla";
    }

    double getCost() const override {
        return coffee->getCost() + 0.80;
    }
};
```

### 2. 텍스트 포맷팅 시스템

```cpp
class TextComponent {
public:
    virtual ~TextComponent() = default;
    virtual std::string getText() const = 0;
    virtual int getLength() const = 0;
};

class PlainText : public TextComponent {
private:
    std::string text;

public:
    explicit PlainText(const std::string& text) : text(text) {}

    std::string getText() const override {
        return text;
    }

    int getLength() const override {
        return text.length();
    }
};

class TextDecorator : public TextComponent {
protected:
    std::unique_ptr<TextComponent> component;

public:
    explicit TextDecorator(std::unique_ptr<TextComponent> component)
        : component(std::move(component)) {}

    std::string getText() const override {
        return component->getText();
    }

    int getLength() const override {
        return component->getLength();
    }
};

class BoldDecorator : public TextDecorator {
public:
    explicit BoldDecorator(std::unique_ptr<TextComponent> component)
        : TextDecorator(std::move(component)) {}

    std::string getText() const override {
        return "<b>" + component->getText() + "</b>";
    }

    int getLength() const override {
        return component->getLength() + 7; // <b></b>
    }
};

class ColorDecorator : public TextDecorator {
private:
    std::string color;

public:
    ColorDecorator(std::unique_ptr<TextComponent> component, const std::string& color)
        : TextDecorator(std::move(component)), color(color) {}

    std::string getText() const override {
        return "<span style=\"color:" + color + "\">" + component->getText() + "</span>";
    }

    int getLength() const override {
        return component->getLength() + 27 + color.length(); // HTML tags
    }
};
```

### 3. 데이터 스트림 처리

```cpp
class DataStream {
public:
    virtual ~DataStream() = default;
    virtual std::string read() = 0;
    virtual void write(const std::string& data) = 0;
    virtual std::string getInfo() const = 0;
};

class FileDataStream : public DataStream {
private:
    std::string filename;
    std::string data;

public:
    explicit FileDataStream(const std::string& filename)
        : filename(filename), data("Sample file content: " + filename) {}

    std::string read() override {
        return data;
    }

    void write(const std::string& newData) override {
        data = newData;
    }

    std::string getInfo() const override {
        return "File: " + filename;
    }
};

class EncryptionDecorator : public DataStreamDecorator {
public:
    explicit EncryptionDecorator(std::unique_ptr<DataStream> stream)
        : DataStreamDecorator(std::move(stream)) {}

    std::string read() override {
        std::string data = stream->read();
        return decrypt(data);
    }

    void write(const std::string& data) override {
        stream->write(encrypt(data));
    }

    std::string getInfo() const override {
        return stream->getInfo() + " [Encrypted]";
    }

private:
    std::string encrypt(const std::string& data) {
        // Simple Caesar cipher for demonstration
        std::string encrypted = data;
        for (char& c : encrypted) {
            if (std::isalpha(c)) {
                c = ((c - 'a' + 3) % 26) + 'a';
            }
        }
        return encrypted;
    }

    std::string decrypt(const std::string& data) {
        // Reverse Caesar cipher
        std::string decrypted = data;
        for (char& c : decrypted) {
            if (std::isalpha(c)) {
                c = ((c - 'a' - 3 + 26) % 26) + 'a';
            }
        }
        return decrypted;
    }
};
```

## 사용 예제

```cpp
int main() {
    // 1. 기본 커피 주문
    auto basicCoffee = std::make_unique<BasicCoffee>("Large");

    // 2. 커피에 우유 추가
    auto coffeeWithMilk = std::make_unique<MilkDecorator>(
        std::make_unique<BasicCoffee>("Regular"));

    // 3. 복합 커피 주문
    auto complexCoffee = std::make_unique<VanillaDecorator>(
        std::make_unique<MilkDecorator>(
            std::make_unique<BasicCoffee>("Large")));

    std::cout << complexCoffee->getDescription() << std::endl;
    // "Large Coffee + Milk + Vanilla"
    std::cout << "$" << complexCoffee->getCost() << std::endl;
    // $5.90 (4.50 + 0.60 + 0.80)

    // 4. 텍스트 포맷팅
    auto styledText = std::make_unique<ColorDecorator>(
        std::make_unique<BoldDecorator>(
            std::make_unique<PlainText>("Hello World")), "red");

    std::cout << styledText->getText() << std::endl;
    // "<span style=\"color:red\"><b>Hello World</b></span>"

    // 5. 데이터 스트림 처리
    auto secureStream = std::make_unique<EncryptionDecorator>(
        std::make_unique<CompressionDecorator>(
            std::make_unique<FileDataStream>("data.txt")));

    secureStream->write("Sensitive data");
    std::string data = secureStream->read();

    return 0;
}
```

## C++의 장점

1. **스마트 포인터 활용**: `std::unique_ptr`로 메모리 관리 자동화
2. **RAII**: 리소스 자동 해제로 메모리 누수 방지
3. **타입 안전성**: 컴파일 타임에 타입 검사
4. **이동 시맨틱**: `std::move`로 효율적인 객체 전달
5. **가상 소멸자**: 적절한 메모리 해제 보장

## 적용 상황

1. **GUI 위젯**: 스크롤바, 테두리, 그림자 등의 추가 기능
2. **I/O 스트림**: 압축, 암호화, 로깅 등의 계층적 처리
3. **웹 미들웨어**: 인증, 캐싱, 압축 등의 요청 처리
4. **게임 객체**: 파워업, 버프/디버프 효과
5. **텍스트 처리**: 폰트, 색상, 스타일 등의 포맷팅

Decorator 패턴은 객체의 기능을 유연하게 확장할 수 있게 해주며, 상속의 복잡성 없이 다양한 조합을 런타임에 구성할 수 있는 강력한 패턴입니다.