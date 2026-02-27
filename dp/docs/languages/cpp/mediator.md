# Mediator Pattern - C++ Implementation

## 개요

Mediator 패턴은 객체들 간의 복잡한 통신과 제어 로직을 중재자 객체로 캡슐화하여 객체 간의 결합도를 낮추는 행위 패턴입니다. 객체들이 서로 직접 참조하지 않고 중재자를 통해 상호작용하게 합니다.

## 구조

- **Mediator**: 컴포넌트 간 통신을 정의하는 인터페이스
- **ConcreteMediator**: 중재자의 구체적 구현
- **Colleague**: 중재자와 통신하는 컴포넌트들

## C++ 구현

```cpp
#include <iostream>
#include <memory>
#include <vector>
#include <string>

class Component;

class Mediator {
public:
    virtual ~Mediator() = default;
    virtual void notify(Component* sender, const std::string& event) = 0;
};

class Component {
protected:
    Mediator* mediator;

public:
    explicit Component(Mediator* mediator = nullptr) : mediator(mediator) {}

    void setMediator(Mediator* m) {
        mediator = m;
    }
};

// Concrete Components
class Button : public Component {
private:
    std::string name;

public:
    Button(const std::string& name, Mediator* mediator = nullptr)
        : Component(mediator), name(name) {}

    void click() {
        std::cout << "Button " << name << " clicked" << std::endl;
        if (mediator) {
            mediator->notify(this, "click");
        }
    }

    std::string getName() const { return name; }
};

class TextBox : public Component {
private:
    std::string text;

public:
    explicit TextBox(Mediator* mediator = nullptr) : Component(mediator) {}

    void setText(const std::string& newText) {
        text = newText;
        std::cout << "TextBox text changed to: " << text << std::endl;
        if (mediator) {
            mediator->notify(this, "textChanged");
        }
    }

    std::string getText() const { return text; }
};

// Concrete Mediator
class DialogMediator : public Mediator {
private:
    Button* submitButton;
    Button* cancelButton;
    TextBox* textBox;

public:
    DialogMediator(Button* submit, Button* cancel, TextBox* text)
        : submitButton(submit), cancelButton(cancel), textBox(text) {

        submitButton->setMediator(this);
        cancelButton->setMediator(this);
        textBox->setMediator(this);
    }

    void notify(Component* sender, const std::string& event) override {
        if (sender == textBox && event == "textChanged") {
            if (textBox->getText().empty()) {
                std::cout << "Submit button disabled (empty text)" << std::endl;
            } else {
                std::cout << "Submit button enabled" << std::endl;
            }
        } else if (sender == submitButton && event == "click") {
            std::cout << "Processing form submission: " << textBox->getText() << std::endl;
        } else if (sender == cancelButton && event == "click") {
            std::cout << "Form cancelled, clearing text" << std::endl;
            textBox->setText("");
        }
    }
};
```

## 사용 예제

```cpp
int main() {
    Button submitBtn("Submit");
    Button cancelBtn("Cancel");
    TextBox textBox;

    DialogMediator dialog(&submitBtn, &cancelBtn, &textBox);

    // Simulate user interactions
    textBox.setText("Hello World");
    submitBtn.click();

    cancelBtn.click();
    submitBtn.click(); // Should show empty text warning

    return 0;
}
```

## C++의 장점

1. **스마트 포인터**: 안전한 참조 관리
2. **캡슐화**: private 멤버로 내부 상태 보호
3. **다형성**: 가상 함수로 유연한 중재

## 적용 상황

1. **GUI 시스템**: 위젯 간 상호작용 관리
2. **챗봇**: 사용자와 시스템 간 중재
3. **워크플로우**: 작업 단계 간 조율
4. **게임**: 플레이어와 게임 요소 간 중재

Mediator 패턴은 복잡한 객체 간 통신을 단순화하고 유지보수성을 크게 향상시킵니다.