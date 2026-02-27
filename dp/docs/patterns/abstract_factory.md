# Abstract Factory Pattern

## 개요
Abstract Factory 패턴은 구체적인 클래스를 지정하지 않고 관련이나 의존적인 객체들의 묶음을 생성할 수 있게 해주는 생성 패턴입니다. 이 패턴은 여러 제품군에 속하는 객체들을 생성할 때 사용되며, 클라이언트 코드를 구체적인 클래스로부터 분리시켜줍니다.

## 구조
- **Abstract Factory (GUIFactory)**: 추상 제품을 생성하는 연산들에 대한 인터페이스를 정의
- **Concrete Factory (WindowsFactory, MacFactory)**: 구체적인 제품을 생성하는 연산들을 구현
- **Abstract Product (Button, Checkbox)**: 제품 객체들에 대한 인터페이스를 정의
- **Concrete Product (WindowsButton, MacButton, WindowsCheckbox, MacCheckbox)**: 구체적인 제품을 정의
- **Client (Application)**: Abstract Factory와 Abstract Product 인터페이스만 사용

## C 언어 구현

### 추상 제품 정의
```c
// 추상 제품 - Button
typedef struct Button {
    void (*paint)(struct Button* self);
    void (*destroy)(struct Button* self);
} Button;

// 추상 제품 - Checkbox
typedef struct Checkbox {
    void (*paint)(struct Checkbox* self);
    void (*destroy)(struct Checkbox* self);
} Checkbox;
```

### 구체적 제품 구현
```c
// Windows 버튼 구현
typedef struct WindowsButton {
    Button base;
} WindowsButton;

void windows_button_paint(Button* button) {
    printf("Rendering Windows Button\n");
}

Button* create_windows_button() {
    WindowsButton* button = malloc(sizeof(WindowsButton));
    button->base.paint = windows_button_paint;
    button->base.destroy = windows_button_destroy;
    return (Button*)button;
}

// Mac 버튼 구현
typedef struct MacButton {
    Button base;
} MacButton;

void mac_button_paint(Button* button) {
    printf("Rendering Mac Button\n");
}

Button* create_mac_button() {
    MacButton* button = malloc(sizeof(MacButton));
    button->base.paint = mac_button_paint;
    button->base.destroy = mac_button_destroy;
    return (Button*)button;
}
```

### 추상 팩토리 정의
```c
typedef struct GUIFactory {
    Button* (*create_button)(struct GUIFactory* self);
    Checkbox* (*create_checkbox)(struct GUIFactory* self);
    void (*destroy)(struct GUIFactory* self);
} GUIFactory;
```

### 구체적 팩토리 구현
```c
// Windows 팩토리
typedef struct WindowsFactory {
    GUIFactory base;
} WindowsFactory;

Button* windows_factory_create_button(GUIFactory* factory) {
    return create_windows_button();
}

Checkbox* windows_factory_create_checkbox(GUIFactory* factory) {
    return create_windows_checkbox();
}

GUIFactory* create_windows_factory() {
    WindowsFactory* factory = malloc(sizeof(WindowsFactory));
    factory->base.create_button = windows_factory_create_button;
    factory->base.create_checkbox = windows_factory_create_checkbox;
    factory->base.destroy = windows_factory_destroy;
    return (GUIFactory*)factory;
}
```

## 사용 예제
```c
// 애플리케이션 생성 및 사용
typedef struct Application {
    GUIFactory* factory;
    Button* button;
    Checkbox* checkbox;
} Application;

Application* create_application(GUIFactory* factory) {
    Application* app = malloc(sizeof(Application));
    app->factory = factory;
    app->button = factory->create_button(factory);
    app->checkbox = factory->create_checkbox(factory);
    return app;
}

// 사용 예제
int main() {
    // Windows 애플리케이션 생성
    GUIFactory* windows_factory = create_windows_factory();
    Application* windows_app = create_application(windows_factory);
    render_ui(windows_app);

    // Mac 애플리케이션 생성
    GUIFactory* mac_factory = create_mac_factory();
    Application* mac_app = create_application(mac_factory);
    render_ui(mac_app);

    return 0;
}
```

## 장점과 단점

### 장점
- **일관성 보장**: 제품군 내의 객체들이 함께 사용되도록 보장
- **구체적 클래스 분리**: 클라이언트 코드를 구체적인 클래스로부터 분리
- **제품군 교체 용이**: 팩토리만 변경하면 전체 제품군을 쉽게 교체
- **확장성**: 새로운 제품군을 추가하기 쉬움

### 단점
- **복잡성 증가**: 많은 인터페이스와 클래스가 필요
- **새로운 제품 추가 어려움**: 새로운 종류의 제품을 추가할 때 모든 팩토리를 수정해야 함
- **메모리 오버헤드**: 함수 포인터와 구조체 계층으로 인한 약간의 오버헤드

## 적용 상황
- **플랫폼별 UI 구성요소**: Windows, Mac, Linux별로 다른 UI 요소가 필요한 경우
- **데이터베이스 연결**: MySQL, PostgreSQL, Oracle 등 다양한 데이터베이스에 대한 연결 객체
- **게임 개발**: 다양한 테마나 스킨에 따른 게임 객체들
- **문서 처리**: PDF, Word, Excel 등 다양한 형식의 문서 처리 객체
- **네트워크 프로토콜**: HTTP, FTP, SMTP 등 다양한 프로토콜별 객체들

이 패턴은 특히 C 언어에서 객체지향적 설계를 구현할 때 유용하며, 함수 포인터를 활용하여 다형성을 구현하는 좋은 예시입니다.