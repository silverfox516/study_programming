# Factory Pattern

## 개요
Factory 패턴은 객체 생성 로직을 별도의 팩토리 클래스나 메소드에 캡슐화하는 생성 패턴입니다. 클라이언트는 구체적인 클래스를 직접 인스턴스화하지 않고 팩토리를 통해 객체를 생성합니다.

## 구조
- **Product**: 생성될 객체들의 공통 인터페이스
- **Concrete Product**: 구체적인 제품 클래스들
- **Factory**: 객체 생성을 담당하는 팩토리

## C 언어 구현

### 제품 인터페이스
```c
typedef enum {
    SHAPE_CIRCLE,
    SHAPE_RECTANGLE,
    SHAPE_TRIANGLE
} ShapeType;

typedef struct Shape {
    void (*draw)(struct Shape*);
    void (*destroy)(struct Shape*);
    double (*calculate_area)(struct Shape*);
} Shape;
```

### 구체적 제품들
```c
typedef struct Circle {
    Shape base;
    double radius;
} Circle;

void circle_draw(Shape* shape) {
    Circle* circle = (Circle*)shape;
    printf("Drawing Circle with radius %.2f\n", circle->radius);
}

double circle_area(Shape* shape) {
    Circle* circle = (Circle*)shape;
    return PI * circle->radius * circle->radius;
}

typedef struct Rectangle {
    Shape base;
    double width, height;
} Rectangle;

void rectangle_draw(Shape* shape) {
    Rectangle* rect = (Rectangle*)shape;
    printf("Drawing Rectangle %.2fx%.2f\n", rect->width, rect->height);
}
```

### 팩토리 구현
```c
// 팩토리 함수
Shape* create_shape(ShapeType type, ...) {
    va_list args;
    va_start(args, type);

    Shape* shape = NULL;

    switch (type) {
        case SHAPE_CIRCLE: {
            double radius = va_arg(args, double);
            Circle* circle = malloc(sizeof(Circle));
            if (circle) {
                circle->base.draw = circle_draw;
                circle->base.calculate_area = circle_area;
                circle->base.destroy = circle_destroy;
                circle->radius = radius;
                shape = (Shape*)circle;
            }
            break;
        }

        case SHAPE_RECTANGLE: {
            double width = va_arg(args, double);
            double height = va_arg(args, double);
            Rectangle* rect = malloc(sizeof(Rectangle));
            if (rect) {
                rect->base.draw = rectangle_draw;
                rect->base.calculate_area = rectangle_area;
                rect->base.destroy = rectangle_destroy;
                rect->width = width;
                rect->height = height;
                shape = (Shape*)rect;
            }
            break;
        }
    }

    va_end(args);
    return shape;
}
```

## 사용 예제
```c
int main() {
    // 팩토리를 통한 객체 생성
    Shape* circle = create_shape(SHAPE_CIRCLE, 5.0);
    Shape* rectangle = create_shape(SHAPE_RECTANGLE, 10.0, 20.0);
    Shape* triangle = create_shape(SHAPE_TRIANGLE, 8.0, 12.0);

    // 동일한 인터페이스로 사용
    circle->draw(circle);
    printf("Area: %.2f\n", circle->calculate_area(circle));

    rectangle->draw(rectangle);
    printf("Area: %.2f\n", rectangle->calculate_area(rectangle));

    // 정리
    circle->destroy(circle);
    rectangle->destroy(rectangle);
    triangle->destroy(triangle);

    return 0;
}
```

## 장점과 단점

### 장점
- **객체 생성 로직 캡슐화**: 생성 로직을 한 곳에 집중
- **유연성**: 새로운 제품 타입을 쉽게 추가
- **클라이언트 단순화**: 구체적인 클래스를 알 필요 없음
- **일관성**: 객체 생성 방식의 표준화

### 단점
- **복잡성 증가**: 간단한 객체 생성에도 팩토리 필요
- **코드 증가**: 팩토리 클래스/함수 추가 필요

## 적용 상황
- **객체 생성이 복잡한 경우**
- **생성할 객체 타입이 런타임에 결정되는 경우**
- **객체 생성 로직을 중앙화하고 싶은 경우**
- **테스트에서 Mock 객체 생성이 필요한 경우**