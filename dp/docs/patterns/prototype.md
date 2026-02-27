# Prototype Pattern

## 개요
Prototype 패턴은 기존 객체를 복사하여 새로운 객체를 생성하는 생성 패턴입니다. 객체의 생성 비용이 클 때 기존 객체를 복사하는 것이 더 효율적인 경우에 사용됩니다.

## C 언어 구현
```c
typedef struct Shape {
    void (*draw)(struct Shape* self);
    struct Shape* (*clone)(struct Shape* self);
    void (*destroy)(struct Shape* self);
} Shape;

typedef struct Circle {
    Shape base;
    double radius;
    char* color;
} Circle;

Shape* circle_clone(Shape* self) {
    Circle* original = (Circle*)self;
    Circle* clone = malloc(sizeof(Circle));

    clone->base = original->base;
    clone->radius = original->radius;
    clone->color = malloc(strlen(original->color) + 1);
    strcpy(clone->color, original->color);

    return (Shape*)clone;
}

void circle_draw(Shape* self) {
    Circle* circle = (Circle*)self;
    printf("Circle: radius=%.2f, color=%s\n", circle->radius, circle->color);
}

Circle* create_circle(double radius, const char* color) {
    Circle* circle = malloc(sizeof(Circle));
    circle->base.draw = circle_draw;
    circle->base.clone = circle_clone;
    circle->base.destroy = circle_destroy;
    circle->radius = radius;
    circle->color = malloc(strlen(color) + 1);
    strcpy(circle->color, color);
    return circle;
}
```

## 사용 예제
```c
Circle* original = create_circle(5.0, "red");
Shape* clone = original->base.clone((Shape*)original);

original->base.draw((Shape*)original);  // Circle: radius=5.00, color=red
clone->draw(clone);                     // Circle: radius=5.00, color=red

// 클론 수정
((Circle*)clone)->radius = 10.0;
clone->draw(clone);                     // Circle: radius=10.00, color=red
```

## 적용 상황
- **복잡한 객체 생성**: 생성 비용이 높은 객체의 복제
- **동적 로딩**: 런타임에 생성할 객체 타입이 결정되는 경우
- **상태 백업**: 객체의 현재 상태를 백업해두고 싶을 때