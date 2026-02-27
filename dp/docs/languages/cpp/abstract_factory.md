# Abstract Factory Pattern - C++ Implementation

## 개요

Abstract Factory 패턴은 관련된 객체들의 패밀리를 생성하기 위한 인터페이스를 제공하는 창조적 디자인 패턴입니다. 이 패턴은 구체적인 클래스들을 지정하지 않고도 객체들의 패밀리를 생성할 수 있게 해줍니다.

C++에서는 순수 가상 함수, 스마트 포인터, 그리고 상속을 통해 타입 안전하고 메모리 효율적인 구현을 제공합니다.

## 구조

### 핵심 구성 요소

1. **Abstract Products** (Button, TextField, Checkbox)
   - 제품군의 각 타입에 대한 추상 인터페이스
   - 순수 가상 함수로 인터페이스 정의

2. **Concrete Products** (WindowsButton, MacButton, LinuxButton 등)
   - Abstract Product 인터페이스의 구체적 구현
   - 플랫폼별 특성을 반영한 구현

3. **Abstract Factory** (UIFactory)
   - 모든 제품 생성 메서드의 추상 인터페이스
   - 제품군 전체의 생성을 담당

4. **Concrete Factories** (WindowsUIFactory, MacUIFactory, LinuxUIFactory)
   - 특정 제품군의 객체들을 생성
   - 일관성 있는 제품군 보장

5. **Client** (Application)
   - Abstract Factory와 Abstract Product 인터페이스만 사용

## C++ 구현

### Abstract Products 정의

```cpp
class Button {
public:
    virtual ~Button() = default;  // 가상 소멸자로 안전한 다형성
    virtual void render() = 0;
    virtual void onClick() = 0;
    virtual std::string getStyle() const = 0;
};

class TextField {
public:
    virtual ~TextField() = default;
    virtual void render() = 0;
    virtual void setValue(const std::string& value) = 0;
    virtual std::string getValue() const = 0;
    virtual std::string getStyle() const = 0;
};
```

### Concrete Products 구현

```cpp
class WindowsButton : public Button {
private:
    std::string text;

public:
    explicit WindowsButton(const std::string& text) : text(text) {}

    void render() override {
        std::cout << "[Windows Button: " << text << "]" << std::endl;
    }

    void onClick() override {
        std::cout << "Windows button clicked with mouse" << std::endl;
    }
};
```

### Abstract Factory 인터페이스

```cpp
class UIFactory {
public:
    virtual ~UIFactory() = default;
    virtual std::unique_ptr<Button> createButton(const std::string& text) = 0;
    virtual std::unique_ptr<TextField> createTextField() = 0;
    virtual std::unique_ptr<Checkbox> createCheckbox() = 0;
    virtual std::string getTheme() const = 0;
};
```

### Concrete Factory 구현

```cpp
class WindowsUIFactory : public UIFactory {
public:
    std::unique_ptr<Button> createButton(const std::string& text) override {
        return std::make_unique<WindowsButton>(text);
    }

    std::unique_ptr<TextField> createTextField() override {
        return std::make_unique<WindowsTextField>();
    }

    std::unique_ptr<Checkbox> createCheckbox() override {
        return std::make_unique<WindowsCheckbox>();
    }
};
```

## 사용 예제

### 클라이언트 코드

```cpp
class Application {
private:
    std::unique_ptr<UIFactory> factory;
    std::vector<std::unique_ptr<Button>> buttons;
    std::vector<std::unique_ptr<TextField>> textFields;

public:
    explicit Application(std::unique_ptr<UIFactory> uiFactory)
        : factory(std::move(uiFactory)) {}

    void createUI() {
        buttons.push_back(factory->createButton("OK"));
        buttons.push_back(factory->createButton("Cancel"));

        textFields.push_back(factory->createTextField());
        textFields[0]->setValue("Username");
    }
};
```

### Factory 생성 함수

```cpp
std::unique_ptr<UIFactory> createUIFactory(const std::string& os) {
    if (os == "Windows") {
        return std::make_unique<WindowsUIFactory>();
    } else if (os == "macOS") {
        return std::make_unique<MacUIFactory>();
    } else if (os == "Linux") {
        return std::make_unique<LinuxUIFactory>();
    }
    throw std::invalid_argument("Unsupported OS: " + os);
}
```

### 실행 결과

```
Creating UI with Windows theme:

Rendering Windows UI:
------------------------
[Windows TextField: Username]
[Windows TextField: Password]
[Windows Checkbox: ☑]
[Windows Checkbox: ☐]
[Windows Button: OK]
[Windows Button: Cancel]

Simulating user interaction:
Windows button 'OK' clicked with mouse
```

## C++의 장점

### 1. 스마트 포인터 활용
- `std::unique_ptr`로 자동 메모리 관리
- 소유권 이전 시맨틱으로 안전한 객체 전달
- 메모리 누수 방지

### 2. 타입 안전성
- 컴파일 타임에 타입 검사
- 가상 함수를 통한 런타임 다형성
- `override` 키워드로 명시적 오버라이딩

### 3. RAII (Resource Acquisition Is Initialization)
- 객체의 생성과 소멸이 자동으로 관리
- 예외 발생 시에도 안전한 리소스 정리

### 4. 효율적인 구현
- 가상 함수 테이블을 통한 효율적 다형성
- Move 시맨틱으로 불필요한 복사 방지

### 5. 템플릿 확장 가능성
- 필요시 템플릿을 활용해 타입 파라미터화 가능
- 컴파일 타임 최적화

## 적용 상황

### 1. 크로스 플랫폼 애플리케이션
- 운영체제별 UI 컴포넌트 생성
- 각 플랫폼의 Look & Feel 유지

### 2. 다양한 데이터베이스 지원
- 데이터베이스별 연결, 쿼리, 트랜잭션 객체 생성
- 데이터베이스 변경 시 최소한의 코드 수정

### 3. 게임 개발
- 플랫폼별 렌더링, 입력, 사운드 시스템
- 하드웨어 차이를 추상화

### 4. 테마 시스템
- 다크모드/라이트모드 UI 컴포넌트
- 브랜드별 디자인 시스템

### 5. 제품군 일관성이 중요한 경우
- 관련 객체들이 함께 사용되어야 하는 상황
- 객체 간 호환성 보장 필요

## 주의사항

1. **복잡성 증가**: 단순한 객체 생성에는 과도할 수 있음
2. **확장성**: 새로운 제품 타입 추가 시 모든 Factory 수정 필요
3. **성능 고려**: 가상 함수 호출 오버헤드
4. **의존성**: 제품군 간의 강한 결합

이 패턴은 객체 생성의 복잡성을 캡슐화하고, 제품군의 일관성을 보장하며, 시스템의 유연성을 높이는 데 매우 효과적입니다.