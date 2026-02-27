# Visitor Pattern - C++ Implementation

## 개요

Visitor 패턴은 알고리즘을 객체 구조에서 분리하여, 기존 요소 클래스를 변경하지 않고 새로운 연산을 추가할 수 있게 하는 행위 패턴입니다. 객체 계층구조에서 다양한 연산을 수행할 때 유용합니다.

## 구조

- **Visitor**: 모든 구체 요소에 대한 visit 메서드 선언
- **ConcreteVisitor**: 구체적인 연산 구현
- **Element**: Visitor를 받아들이는 accept 메서드 선언
- **ConcreteElement**: 구체적인 요소 구현

## C++ 구현

### 1. 기본 Visitor 패턴

```cpp
#include <iostream>
#include <memory>
#include <vector>
#include <string>

// Forward declarations
class Circle;
class Rectangle;
class Triangle;

// Visitor interface
class ShapeVisitor {
public:
    virtual ~ShapeVisitor() = default;
    virtual void visit(const Circle& circle) = 0;
    virtual void visit(const Rectangle& rectangle) = 0;
    virtual void visit(const Triangle& triangle) = 0;
};

// Element interface
class Shape {
public:
    virtual ~Shape() = default;
    virtual void accept(ShapeVisitor& visitor) const = 0;
};

// Concrete Elements
class Circle : public Shape {
private:
    double radius;
    
public:
    explicit Circle(double radius) : radius(radius) {}
    
    void accept(ShapeVisitor& visitor) const override {
        visitor.visit(*this);
    }
    
    double getRadius() const { return radius; }
};

class Rectangle : public Shape {
private:
    double width, height;
    
public:
    Rectangle(double width, double height) : width(width), height(height) {}
    
    void accept(ShapeVisitor& visitor) const override {
        visitor.visit(*this);
    }
    
    double getWidth() const { return width; }
    double getHeight() const { return height; }
};

// Concrete Visitors
class AreaCalculatorVisitor : public ShapeVisitor {
private:
    double totalArea = 0.0;
    
public:
    void visit(const Circle& circle) override {
        double area = 3.14159 * circle.getRadius() * circle.getRadius();
        totalArea += area;
        std::cout << "Circle area: " << area << std::endl;
    }
    
    void visit(const Rectangle& rectangle) override {
        double area = rectangle.getWidth() * rectangle.getHeight();
        totalArea += area;
        std::cout << "Rectangle area: " << area << std::endl;
    }
    
    void visit(const Triangle& triangle) override {
        // Implementation for triangle
    }
    
    double getTotalArea() const { return totalArea; }
};

class DrawingVisitor : public ShapeVisitor {
public:
    void visit(const Circle& circle) override {
        std::cout << "Drawing circle with radius " << circle.getRadius() << std::endl;
    }
    
    void visit(const Rectangle& rectangle) override {
        std::cout << "Drawing rectangle " << rectangle.getWidth() 
                  << "x" << rectangle.getHeight() << std::endl;
    }
    
    void visit(const Triangle& triangle) override {
        std::cout << "Drawing triangle" << std::endl;
    }
};
```

### 2. 고급 예제 - 컴파일러 AST

```cpp
// AST Node types
class ASTVisitor;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
};

class NumberNode : public ASTNode {
private:
    int value;
    
public:
    explicit NumberNode(int value) : value(value) {}
    void accept(ASTVisitor& visitor) override;
    int getValue() const { return value; }
};

class AddNode : public ASTNode {
private:
    std::unique_ptr<ASTNode> left, right;
    
public:
    AddNode(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
        : left(std::move(left)), right(std::move(right)) {}
        
    void accept(ASTVisitor& visitor) override;
    ASTNode* getLeft() const { return left.get(); }
    ASTNode* getRight() const { return right.get(); }
};

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(NumberNode& node) = 0;
    virtual void visit(AddNode& node) = 0;
};

// Implementations
void NumberNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void AddNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// Concrete Visitors
class EvaluatorVisitor : public ASTVisitor {
private:
    int result = 0;
    
public:
    void visit(NumberNode& node) override {
        result = node.getValue();
    }
    
    void visit(AddNode& node) override {
        node.getLeft()->accept(*this);
        int leftResult = result;
        
        node.getRight()->accept(*this);
        int rightResult = result;
        
        result = leftResult + rightResult;
    }
    
    int getResult() const { return result; }
};

class PrinterVisitor : public ASTVisitor {
public:
    void visit(NumberNode& node) override {
        std::cout << node.getValue();
    }
    
    void visit(AddNode& node) override {
        std::cout << "(";
        node.getLeft()->accept(*this);
        std::cout << " + ";
        node.getRight()->accept(*this);
        std::cout << ")";
    }
};
```

## 사용 예제

```cpp
int main() {
    // 1. Shape 예제
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(5.0));
    shapes.push_back(std::make_unique<Rectangle>(4.0, 3.0));
    
    AreaCalculatorVisitor areaCalc;
    DrawingVisitor drawer;
    
    for (const auto& shape : shapes) {
        shape->accept(areaCalc);
        shape->accept(drawer);
    }
    
    std::cout << "Total area: " << areaCalc.getTotalArea() << std::endl;
    
    // 2. AST 예제
    // (5 + 3) 표현
    auto ast = std::make_unique<AddNode>(
        std::make_unique<NumberNode>(5),
        std::make_unique<NumberNode>(3)
    );
    
    EvaluatorVisitor evaluator;
    PrinterVisitor printer;
    
    ast->accept(printer);
    std::cout << " = ";
    
    ast->accept(evaluator);
    std::cout << evaluator.getResult() << std::endl;
    
    return 0;
}
```

## C++의 장점

1. **타입 안전성**: 컴파일 타임에 타입 체크
2. **성능**: 가상 함수 호출 최소화
3. **메모리 관리**: RAII로 안전한 리소스 관리

## 적용 상황

1. **컴파일러**: AST 순회, 코드 생성
2. **문서 처리**: XML/HTML 파싱
3. **그래픽스**: 도형 렌더링, 변환
4. **게임**: 아이템 효과, 스킬 시스템

Visitor 패턴은 객체 계층구조에 다양한 연산을 추가할 때 매우 유용한 패턴입니다.