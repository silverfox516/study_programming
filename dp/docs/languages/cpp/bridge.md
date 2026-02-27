# Bridge Pattern - C++ Implementation

## 개요

Bridge 패턴은 추상화와 구현을 분리하여 각각 독립적으로 변경할 수 있도록 하는 구조적 디자인 패턴입니다. 이 패턴을 통해 런타임에 구현을 선택하거나 교체할 수 있으며, 추상화와 구현 모두의 확장성을 보장합니다.

C++에서는 상속과 컴포지션을 조합하여 강력한 Bridge 패턴을 구현할 수 있으며, 스마트 포인터를 통한 안전한 메모리 관리와 다형성을 제공합니다.

## 구조

### 핵심 구성 요소

1. **Abstraction** (Shape, Notification)
   - 고수준의 제어 로직을 정의
   - Implementor에 대한 참조를 유지
   - 클라이언트가 사용하는 인터페이스

2. **Refined Abstraction** (Circle, Rectangle, Line, UrgentNotification)
   - Abstraction을 확장하여 더 구체적인 변형 제공
   - 추가 기능이나 특수한 동작 구현

3. **Implementor** (DrawingAPI, NotificationSender)
   - 모든 구체적 구현 클래스의 공통 인터페이스
   - 추상화와 다른 인터페이스를 가질 수 있음

4. **Concrete Implementor** (OpenGLRenderer, DirectXRenderer, SVGRenderer)
   - Implementor 인터페이스의 구체적 구현
   - 플랫폼별 또는 알고리즘별 구현 제공

## C++ 구현

### 1. Implementation Interface (Bridge) 정의

```cpp
class DrawingAPI {
public:
    virtual ~DrawingAPI() = default;
    virtual void drawLine(double x1, double y1, double x2, double y2) = 0;
    virtual void drawCircle(double x, double y, double radius) = 0;
    virtual void drawRectangle(double x, double y, double width, double height) = 0;
    virtual void setColor(const std::string& color) = 0;
    virtual std::string getRendererName() const = 0;
};
```

### 2. Concrete Implementation 클래스들

#### OpenGL 렌더러
```cpp
class OpenGLRenderer : public DrawingAPI {
private:
    std::string currentColor = "white";

public:
    void drawCircle(double x, double y, double radius) override {
        std::cout << "[OpenGL] Drawing circle at (" << x << "," << y
                  << ") with radius " << radius << " in " << currentColor << std::endl;
    }

    void setColor(const std::string& color) override {
        currentColor = color;
        std::cout << "[OpenGL] Color set to " << color << std::endl;
    }

    std::string getRendererName() const override {
        return "OpenGL";
    }
};
```

#### DirectX 렌더러
```cpp
class DirectXRenderer : public DrawingAPI {
private:
    std::string currentColor = "white";

public:
    void drawCircle(double x, double y, double radius) override {
        std::cout << "[DirectX] Rendering circle: center=(" << x << "," << y
                  << ") r=" << radius << " color=" << currentColor << std::endl;
    }

    // 다른 메서드들...
};
```

#### SVG 렌더러
```cpp
class SVGRenderer : public DrawingAPI {
private:
    std::string currentColor = "black";
    std::ostringstream svgContent;  // SVG 콘텐츠 누적

public:
    void drawCircle(double x, double y, double radius) override {
        svgContent << "<circle cx=\"" << x << "\" cy=\"" << y
                   << "\" r=\"" << radius << "\" fill=\"" << currentColor << "\"/>\n";
        std::cout << "[SVG] Added circle element" << std::endl;
    }

    std::string getSVGContent() const {
        return "<svg>\n" + svgContent.str() + "</svg>";
    }
};
```

### 3. Abstraction 클래스

```cpp
class Shape {
protected:
    std::unique_ptr<DrawingAPI> drawingAPI;  // Bridge to implementation
    double x, y;

public:
    Shape(std::unique_ptr<DrawingAPI> api, double x = 0, double y = 0)
        : drawingAPI(std::move(api)), x(x), y(y) {}

    virtual ~Shape() = default;
    virtual void draw() = 0;  // 추상 메서드

    virtual void move(double newX, double newY) {
        x = newX;
        y = newY;
    }

    void setColor(const std::string& color) {
        drawingAPI->setColor(color);  // Bridge 사용
    }

    std::string getRendererName() const {
        return drawingAPI->getRendererName();
    }
};
```

### 4. Refined Abstraction 클래스들

#### Circle 클래스
```cpp
class Circle : public Shape {
private:
    double radius;

public:
    Circle(std::unique_ptr<DrawingAPI> api, double x, double y, double radius)
        : Shape(std::move(api), x, y), radius(radius) {}

    void draw() override {
        drawingAPI->drawCircle(x, y, radius);  // Bridge를 통한 구현 호출
    }

    void setRadius(double newRadius) {
        radius = newRadius;
    }

    double getRadius() const {
        return radius;
    }
};
```

#### 복잡한 도형 (House)
```cpp
class House : public Shape {
public:
    House(std::unique_ptr<DrawingAPI> api, double x, double y)
        : Shape(std::move(api), x, y) {}

    void draw() override {
        std::cout << "Drawing house at (" << x << "," << y << ") using "
                  << drawingAPI->getRendererName() << std::endl;

        // 집의 각 부분을 그리기
        drawingAPI->setColor("brown");
        drawingAPI->drawRectangle(x, y, 100, 80);       // 기초

        drawingAPI->setColor("red");
        drawingAPI->drawRectangle(x - 10, y + 80, 120, 40);  // 지붕

        drawingAPI->setColor("darkbrown");
        drawingAPI->drawRectangle(x + 40, y, 20, 50);   // 문

        drawingAPI->setColor("lightblue");
        drawingAPI->drawCircle(x + 20, y + 60, 8);      // 창문들
        drawingAPI->drawCircle(x + 80, y + 60, 8);
    }
};
```

### 5. 알림 시스템 Bridge 예제

#### 알림 발송 인터페이스
```cpp
class NotificationSender {
public:
    virtual ~NotificationSender() = default;
    virtual void sendNotification(const std::string& title,
                                  const std::string& message) = 0;
    virtual std::string getChannelName() const = 0;
};
```

#### 구체적 발송 방법들
```cpp
class EmailSender : public NotificationSender {
public:
    void sendNotification(const std::string& title,
                         const std::string& message) override {
        std::cout << "[EMAIL] To: user@example.com" << std::endl;
        std::cout << "[EMAIL] Subject: " << title << std::endl;
        std::cout << "[EMAIL] Body: " << message << std::endl;
    }

    std::string getChannelName() const override {
        return "Email";
    }
};

class SMSSender : public NotificationSender {
public:
    void sendNotification(const std::string& title,
                         const std::string& message) override {
        std::cout << "[SMS] " << title << ": " << message
                  << " (160 char limit)" << std::endl;
    }
};
```

#### 알림 추상화
```cpp
class UrgentNotification : public Notification {
public:
    UrgentNotification(std::unique_ptr<NotificationSender> sender,
                      const std::string& title, const std::string& message)
        : Notification(std::move(sender), title, message) {}

    void send() override {
        std::string urgentTitle = "🚨 URGENT: " + title;
        std::string urgentMessage = "⚠️ " + message + " ⚠️";
        sender->sendNotification(urgentTitle, urgentMessage);
    }
};
```

## 사용 예제

### 1. 그래픽 렌더링
```cpp
// 다양한 렌더러를 사용한 도형 생성
std::vector<std::unique_ptr<Shape>> shapes;

shapes.push_back(std::make_unique<Circle>(
    std::make_unique<OpenGLRenderer>(), 10, 10, 5));
shapes.push_back(std::make_unique<Rectangle>(
    std::make_unique<DirectXRenderer>(), 20, 20, 15, 10));
shapes.push_back(std::make_unique<Line>(
    std::make_unique<SVGRenderer>(), 0, 0, 50, 50));

for (auto& shape : shapes) {
    std::cout << "Using " << shape->getRendererName() << ":" << std::endl;
    shape->setColor("blue");
    shape->draw();
}
```

### 2. SVG 콘텐츠 생성
```cpp
auto svgRenderer = std::make_unique<SVGRenderer>();
auto svgRendererPtr = svgRenderer.get();

auto svgCircle = std::make_unique<Circle>(std::move(svgRenderer), 50, 50, 25);
svgCircle->setColor("red");
svgCircle->draw();

std::cout << "Generated SVG:" << std::endl;
std::cout << svgRendererPtr->getSVGContent() << std::endl;
```

### 3. 알림 시스템
```cpp
std::vector<std::unique_ptr<Notification>> notifications;

notifications.push_back(std::make_unique<SimpleNotification>(
    std::make_unique<EmailSender>(),
    "Welcome", "Thank you for signing up!"));

notifications.push_back(std::make_unique<UrgentNotification>(
    std::make_unique<SMSSender>(),
    "Security Alert", "Suspicious login detected"));

for (auto& notification : notifications) {
    std::cout << "Sending via " << notification->getChannelName() << std::endl;
    notification->send();
}
```

### 실행 결과
```
Using OpenGL:
[OpenGL] Color set to blue
[OpenGL] Drawing circle at (10,10) with radius 5 in blue

Using DirectX:
[DirectX] Color changed to blue
[DirectX] Rendering rectangle: pos=(20,20) size=15x10 color=blue

Sending via Email:
[EMAIL] To: user@example.com
[EMAIL] Subject: Welcome
[EMAIL] Body: Thank you for signing up!

Sending via SMS:
[SMS] 🚨 URGENT: Security Alert: ⚠️ Suspicious login detected ⚠️
```

## C++의 장점

### 1. 스마트 포인터를 통한 안전한 메모리 관리
```cpp
std::unique_ptr<DrawingAPI> drawingAPI;  // 자동 메모리 관리
```

### 2. Move 시맨틱으로 효율적인 객체 전달
```cpp
Shape(std::unique_ptr<DrawingAPI> api, double x = 0, double y = 0)
    : drawingAPI(std::move(api)), x(x), y(y) {}  // 소유권 이전
```

### 3. 가상 소멸자로 안전한 다형성
```cpp
virtual ~DrawingAPI() = default;  // 안전한 다형적 소멸
```

### 4. 템플릿 활용 가능성
```cpp
template<typename RendererType>
class TemplatedShape {
    std::unique_ptr<RendererType> renderer;
    // 컴파일 타임 타입 안전성
};
```

### 5. RAII와 예외 안전성
- 자원의 자동 획득과 해제
- 예외 발생 시에도 안전한 정리

### 6. 다중 상속 지원 (필요 시)
```cpp
class AdvancedRenderer : public DrawingAPI, public Logger {
    // 여러 인터페이스 구현 가능
};
```

## 적용 상황

### 1. 그래픽스 시스템
- 다양한 렌더링 API (OpenGL, DirectX, Vulkan)
- 플랫폼별 렌더링 엔진
- 2D/3D 그래픽스 추상화

### 2. 데이터베이스 드라이버
- 다양한 데이터베이스 (MySQL, PostgreSQL, SQLite)
- 동일한 쿼리 인터페이스
- 데이터베이스별 최적화

### 3. 통신 프로토콜
- 다양한 네트워크 프로토콜 (TCP, UDP, HTTP)
- 메시지 전송 추상화
- 프로토콜별 구현

### 4. 파일 시스템
- 다양한 파일 형식 (JSON, XML, Binary)
- 저장/로드 추상화
- 형식별 직렬화

### 5. UI 프레임워크
- 다양한 플랫폼 (Windows, macOS, Linux)
- 네이티브 UI 컨트롤
- 크로스 플랫폼 추상화

## 장점과 단점

### 장점
1. **플랫폼 독립성**: 구현과 추상화의 분리
2. **확장성**: 새로운 구현 추가가 용이
3. **런타임 교체**: 동적으로 구현 변경 가능
4. **캡슐화**: 클라이언트로부터 구현 세부사항 숨김
5. **재사용성**: 동일한 추상화를 다양한 구현과 결합

### 단점
1. **복잡성**: 추가적인 간접화 계층
2. **성능**: 가상 함수 호출로 인한 약간의 오버헤드
3. **설계 복잡도**: 올바른 추상화 경계 설정 필요

## 관련 패턴

- **Adapter**: 기존 인터페이스 변환 (vs Bridge: 설계 단계부터 분리)
- **Strategy**: 알고리즘 교체 (vs Bridge: 전체 구현 계층 분리)
- **Abstract Factory**: 제품군 생성 (Bridge와 함께 사용 가능)
- **State**: 상태별 행동 변경 (vs Bridge: 구현 방법 변경)

Bridge 패턴은 추상화와 구현의 독립성을 보장하여, 유지보수가 용이하고 확장 가능한 시스템을 만드는 데 매우 효과적입니다.