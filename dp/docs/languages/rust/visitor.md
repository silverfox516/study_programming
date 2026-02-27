# Visitor Pattern - Rust Implementation

## 개요

Visitor 패턴은 객체 구조의 요소에 수행할 연산을 분리하여 정의할 수 있게 해줍니다. 새로운 연산을 기존 객체 구조를 변경하지 않고 정의할 수 있습니다.

## 구조

```rust
// Visitor trait
pub trait ShapeVisitor {
    fn visit_circle(&mut self, circle: &Circle) -> String;
    fn visit_rectangle(&mut self, rectangle: &Rectangle) -> String;
    fn visit_triangle(&mut self, triangle: &Triangle) -> String;
}

// Visitable trait
pub trait Shape {
    fn accept(&self, visitor: &mut dyn ShapeVisitor) -> String;
}

// Concrete visitors
pub struct AreaCalculator {
    total_area: f64,
}

impl ShapeVisitor for AreaCalculator {
    fn visit_circle(&mut self, circle: &Circle) -> String {
        let area = std::f64::consts::PI * circle.radius * circle.radius;
        self.total_area += area;
        format!("Circle area: {:.2}", area)
    }

    fn visit_rectangle(&mut self, rectangle: &Rectangle) -> String {
        let area = rectangle.width * rectangle.height;
        self.total_area += area;
        format!("Rectangle area: {:.2}", area)
    }

    fn visit_triangle(&mut self, triangle: &Triangle) -> String {
        let area = 0.5 * triangle.base * triangle.height;
        self.total_area += area;
        format!("Triangle area: {:.2}", area)
    }
}
```

## 적용 상황

### 1. 추상 구문 트리 처리
### 2. 컴파일러 최적화
### 3. 파일 시스템 순회
### 4. 그래픽 요소 처리
### 5. 문서 구조 분석