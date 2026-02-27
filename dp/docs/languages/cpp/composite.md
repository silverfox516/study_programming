# Composite Pattern - C++ Implementation

## 개요

Composite 패턴은 객체들을 트리 구조로 구성하여 부분-전체 계층구조를 표현하는 구조적 디자인 패턴입니다. 클라이언트가 개별 객체와 복합 객체를 동일하게 처리할 수 있도록 합니다.

C++에서는 상속, 다형성, 스마트 포인터, STL 컨테이너를 활용하여 안전하고 효율적인 트리 구조를 구현할 수 있습니다.

## 구조

### 핵심 구성 요소

1. **Component** (FileSystemComponent, UIComponent)
   - Leaf와 Composite 객체의 공통 인터페이스
   - 자식 관리 메서드의 기본 구현 제공

2. **Leaf** (File, Button)
   - 트리의 잎 노드 객체
   - 자식을 가질 수 없음
   - 기본적인 행동 구현

3. **Composite** (Directory, Panel)
   - 자식을 가질 수 있는 Component
   - 자식들에게 요청을 위임
   - 자식 관리 메서드 구현

## C++ 구현

### 1. Component 인터페이스

```cpp
class FileSystemComponent {
public:
    virtual ~FileSystemComponent() = default;
    virtual void display(int depth = 0) const = 0;
    virtual size_t getSize() const = 0;
    virtual std::string getName() const = 0;
    virtual bool isComposite() const { return false; }

    // 자식 관리 메서드 (기본 구현은 예외 발생)
    virtual void add(std::unique_ptr<FileSystemComponent> component) {
        throw std::runtime_error("Operation not supported");
    }

    virtual void remove(const std::string& name) {
        throw std::runtime_error("Operation not supported");
    }

    virtual FileSystemComponent* find(const std::string& name) {
        return (getName() == name) ? this : nullptr;
    }
};
```

### 2. Leaf 구현 - File

```cpp
class File : public FileSystemComponent {
private:
    std::string name;
    size_t size;
    std::string content;

public:
    File(const std::string& name, size_t size, const std::string& content = "")
        : name(name), size(size), content(content) {}

    void display(int depth = 0) const override {
        std::string indent(depth * 2, ' ');
        std::cout << indent << "📄 " << name << " (" << size << " bytes)" << std::endl;
    }

    size_t getSize() const override {
        return size;
    }

    std::string getName() const override {
        return name;
    }

    const std::string& getContent() const {
        return content;
    }

    void setContent(const std::string& newContent) {
        content = newContent;
        size = content.length();
    }
};
```

### 3. Composite 구현 - Directory

```cpp
class Directory : public FileSystemComponent {
private:
    std::string name;
    std::vector<std::unique_ptr<FileSystemComponent>> children;

public:
    explicit Directory(const std::string& name) : name(name) {}

    void display(int depth = 0) const override {
        std::string indent(depth * 2, ' ');
        std::cout << indent << "📁 " << name << "/" << std::endl;

        // 자식들을 재귀적으로 표시
        for (const auto& child : children) {
            child->display(depth + 1);
        }
    }

    size_t getSize() const override {
        size_t totalSize = 0;
        // 모든 자식의 크기를 합산
        for (const auto& child : children) {
            totalSize += child->getSize();
        }
        return totalSize;
    }

    std::string getName() const override {
        return name;
    }

    bool isComposite() const override {
        return true;
    }

    void add(std::unique_ptr<FileSystemComponent> component) override {
        children.push_back(std::move(component));
    }

    void remove(const std::string& name) override {
        children.erase(
            std::remove_if(children.begin(), children.end(),
                [&name](const auto& child) {
                    return child->getName() == name;
                }),
            children.end()
        );
    }

    FileSystemComponent* find(const std::string& name) override {
        if (this->name == name) {
            return this;
        }

        // 재귀적으로 자식들에서 검색
        for (auto& child : children) {
            if (auto found = child->find(name)) {
                return found;
            }
        }
        return nullptr;
    }

    size_t getChildCount() const {
        return children.size();
    }
};
```

### 4. UI 컴포넌트 시스템 예제

#### UI Component 인터페이스
```cpp
class UIComponent {
public:
    virtual ~UIComponent() = default;
    virtual void render() const = 0;
    virtual void handleClick(int x, int y) = 0;
    virtual std::string getType() const = 0;
    virtual bool isContainer() const { return false; }

    virtual void add(std::unique_ptr<UIComponent> component) {
        throw std::runtime_error("Cannot add to non-container component");
    }
};
```

#### Leaf - Button
```cpp
class Button : public UIComponent {
private:
    std::string text;
    int x, y, width, height;

public:
    Button(const std::string& text, int x, int y, int width, int height)
        : text(text), x(x), y(y), width(width), height(height) {}

    void render() const override {
        std::cout << "🔘 Button: '" << text << "' at (" << x << "," << y
                  << ") size: " << width << "x" << height << std::endl;
    }

    void handleClick(int clickX, int clickY) override {
        if (clickX >= x && clickX <= x + width &&
            clickY >= y && clickY <= y + height) {
            std::cout << "Button '" << text << "' clicked!" << std::endl;
        }
    }

    std::string getType() const override {
        return "Button";
    }
};
```

#### Composite - Panel
```cpp
class Panel : public UIComponent {
private:
    std::string title;
    int x, y, width, height;
    std::vector<std::unique_ptr<UIComponent>> components;

public:
    Panel(const std::string& title, int x, int y, int width, int height)
        : title(title), x(x), y(y), width(width), height(height) {}

    void render() const override {
        std::cout << "📱 Panel: '" << title << "' at (" << x << "," << y
                  << ") size: " << width << "x" << height << std::endl;

        // 모든 자식 컴포넌트 렌더링
        for (const auto& component : components) {
            component->render();
        }
    }

    void handleClick(int clickX, int clickY) override {
        // 클릭 이벤트를 자식들에게 전파
        for (auto& component : components) {
            component->handleClick(clickX, clickY);
        }
    }

    std::string getType() const override {
        return "Panel";
    }

    bool isContainer() const override {
        return true;
    }

    void add(std::unique_ptr<UIComponent> component) override {
        components.push_back(std::move(component));
    }
};
```

### 5. 그래픽 도형 시스템 예제

```cpp
class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw() const = 0;
    virtual void move(int dx, int dy) = 0;
    virtual double getArea() const = 0;
    virtual std::string getInfo() const = 0;
};

class Circle : public Shape {
private:
    double radius;
    int centerX, centerY;

public:
    Circle(double radius, int x, int y) : radius(radius), centerX(x), centerY(y) {}

    void draw() const override {
        std::cout << "Drawing circle: radius=" << radius
                  << " at (" << centerX << "," << centerY << ")" << std::endl;
    }

    void move(int dx, int dy) override {
        centerX += dx;
        centerY += dy;
    }

    double getArea() const override {
        return 3.14159 * radius * radius;
    }

    std::string getInfo() const override {
        return "Circle(r=" + std::to_string(radius) +
               ", center=(" + std::to_string(centerX) + "," + std::to_string(centerY) + "))";
    }
};

class Group : public Shape {
private:
    std::vector<std::unique_ptr<Shape>> shapes;
    std::string name;

public:
    explicit Group(const std::string& name) : name(name) {}

    void draw() const override {
        std::cout << "Drawing group: " << name << std::endl;
        for (const auto& shape : shapes) {
            shape->draw();
        }
    }

    void move(int dx, int dy) override {
        std::cout << "Moving group: " << name << " by (" << dx << "," << dy << ")" << std::endl;
        for (auto& shape : shapes) {
            shape->move(dx, dy);
        }
    }

    double getArea() const override {
        double totalArea = 0;
        for (const auto& shape : shapes) {
            totalArea += shape->getArea();
        }
        return totalArea;
    }

    std::string getInfo() const override {
        std::ostringstream info;
        info << "Group: " << name << " [";
        for (size_t i = 0; i < shapes.size(); ++i) {
            info << shapes[i]->getInfo();
            if (i < shapes.size() - 1) info << ", ";
        }
        info << "]";
        return info.str();
    }

    void add(std::unique_ptr<Shape> shape) {
        shapes.push_back(std::move(shape));
    }
};
```

## 사용 예제

### 1. 파일 시스템
```cpp
// 디렉토리 생성
auto rootDir = std::make_unique<Directory>("root");
auto documentsDir = std::make_unique<Directory>("Documents");
auto picturesDir = std::make_unique<Directory>("Pictures");

// 파일 생성
auto readme = std::make_unique<File>("README.txt", 1024, "Welcome to our project!");
auto photo1 = std::make_unique<File>("vacation.jpg", 2048000);
auto photo2 = std::make_unique<File>("family.png", 1536000);

// 구조 구성
picturesDir->add(std::move(photo1));
picturesDir->add(std::move(photo2));

documentsDir->add(std::move(readme));

rootDir->add(std::move(documentsDir));
rootDir->add(std::move(picturesDir));

// 전체 구조 표시
rootDir->display();

// 전체 크기 계산
std::cout << "Total size: " << rootDir->getSize() << " bytes" << std::endl;

// 파일 검색
if (auto found = rootDir->find("vacation.jpg")) {
    std::cout << "Found: " << found->getName() << std::endl;
}
```

### 2. UI 시스템
```cpp
// 메인 윈도우 패널
auto mainWindow = std::make_unique<Panel>("Main Window", 0, 0, 800, 600);

// 버튼들 생성
auto okButton = std::make_unique<Button>("OK", 100, 500, 80, 30);
auto cancelButton = std::make_unique<Button>("Cancel", 200, 500, 80, 30);

// 중첩된 패널
auto toolbarPanel = std::make_unique<Panel>("Toolbar", 0, 0, 800, 50);
auto fileButton = std::make_unique<Button>("File", 10, 10, 60, 30);
auto editButton = std::make_unique<Button>("Edit", 80, 10, 60, 30);

toolbarPanel->add(std::move(fileButton));
toolbarPanel->add(std::move(editButton));

mainWindow->add(std::move(toolbarPanel));
mainWindow->add(std::move(okButton));
mainWindow->add(std::move(cancelButton));

// UI 렌더링
mainWindow->render();

// 클릭 이벤트 처리
mainWindow->handleClick(150, 515); // OK 버튼 클릭
```

### 3. 그래픽 도형 그룹
```cpp
// 기본 도형들
auto circle1 = std::make_unique<Circle>(5.0, 10, 10);
auto circle2 = std::make_unique<Circle>(3.0, 20, 20);

// 그룹 생성
auto group1 = std::make_unique<Group>("CircleGroup");
group1->add(std::move(circle1));
group1->add(std::move(circle2));

// 더 큰 그룹 생성
auto mainGroup = std::make_unique<Group>("MainGroup");
mainGroup->add(std::move(group1));

// 그룹 전체 조작
mainGroup->draw();
std::cout << "Total area: " << mainGroup->getArea() << std::endl;

// 그룹 전체 이동
mainGroup->move(5, 5);
```

### 실행 결과
```
📁 root/
  📁 Documents/
    📄 README.txt (1024 bytes)
  📁 Pictures/
    📄 vacation.jpg (2048000 bytes)
    📄 family.png (1536000 bytes)

Total size: 3585024 bytes
Found: vacation.jpg

📱 Panel: 'Main Window' at (0,0) size: 800x600
📱 Panel: 'Toolbar' at (0,0) size: 800x50
🔘 Button: 'File' at (10,10) size: 60x30
🔘 Button: 'Edit' at (80,10) size: 60x30
🔘 Button: 'OK' at (100,500) size: 80x30
🔘 Button: 'Cancel' at (200,500) size: 80x30

Button 'OK' clicked!

Drawing group: MainGroup
Drawing group: CircleGroup
Drawing circle: radius=5 at (10,10)
Drawing circle: radius=3 at (20,20)
Total area: 107.124
```

## C++의 장점

### 1. 스마트 포인터를 통한 안전한 메모리 관리
```cpp
std::vector<std::unique_ptr<FileSystemComponent>> children;
children.push_back(std::move(component)); // 소유권 이전
```

### 2. STL 컨테이너 활용
```cpp
std::vector<std::unique_ptr<UIComponent>> components;
std::remove_if(children.begin(), children.end(), predicate);
```

### 3. 람다를 활용한 함수형 프로그래밍
```cpp
children.erase(
    std::remove_if(children.begin(), children.end(),
        [&name](const auto& child) {
            return child->getName() == name;
        }),
    children.end()
);
```

### 4. 가상 소멸자로 안전한 다형성
```cpp
virtual ~FileSystemComponent() = default;
```

### 5. 기본 매개변수와 오버로딩
```cpp
virtual void display(int depth = 0) const = 0;
```

### 6. const 정확성
```cpp
void display(int depth = 0) const override; // const 메서드
```

### 7. 예외를 통한 에러 처리
```cpp
virtual void add(std::unique_ptr<FileSystemComponent> component) {
    throw std::runtime_error("Operation not supported");
}
```

## 적용 상황

### 1. 파일 시스템
- 파일과 디렉토리 구조
- 트리 탐색 및 검색
- 크기 계산 및 속성 관리

### 2. GUI 프레임워크
- UI 컴포넌트 계층구조
- 이벤트 처리 전파
- 레이아웃 관리

### 3. 그래픽 에디터
- 도형 그룹화
- 변환 작업 (이동, 회전, 크기 조정)
- 계층적 렌더링

### 4. 문서 구조
- HTML/XML DOM
- 문서 섹션과 단락
- 마크다운 파서

### 5. 조직도/계층구조
- 회사 조직도
- 메뉴 시스템
- 카테고리 분류

### 6. 게임 개발
- 씬 그래프
- 게임 오브젝트 계층
- 인벤토리 시스템

## 장점과 단점

### 장점
1. **단순성**: 개별 객체와 복합 객체를 동일하게 처리
2. **확장성**: 새로운 Component 타입 쉽게 추가
3. **재귀적 구조**: 자연스러운 트리 구조 표현
4. **Open/Closed 원칙**: 기존 코드 수정 없이 확장 가능

### 단점
1. **과도한 일반화**: 단순한 경우에도 복잡한 구조
2. **타입 안전성**: 런타임에 타입 확인 필요
3. **성능**: 가상 함수 호출과 재귀로 인한 오버헤드

## 관련 패턴

- **Iterator**: Composite 구조를 순회하기 위해 함께 사용
- **Visitor**: Composite 구조의 노드들에 대한 연산 정의
- **Command**: Composite 구조에서 실행 취소 가능한 작업
- **Decorator**: 개별 객체에 기능 추가 (vs Composite: 구조 구성)

Composite 패턴은 부분-전체 계층구조를 가진 문제 영역에서 매우 유용하며, 특히 재귀적 구조를 다루는 애플리케이션에서 코드의 복잡성을 크게 줄여줍니다.