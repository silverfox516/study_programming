# Bridge Pattern

## 개요
Bridge 패턴은 구현부에서 추상층을 분리하여 각자 독립적으로 변화할 수 있게 해주는 구조 패턴입니다. 이 패턴은 상속보다는 구성(composition)을 통해 플랫폼 독립성을 제공하고, 런타임에 구현을 변경할 수 있도록 해줍니다.

## 구조
- **Abstraction (Shape)**: 추상화 계층의 인터페이스를 정의하고 구현 객체에 대한 참조를 유지
- **Refined Abstraction (Circle, Rectangle)**: 추상화를 확장하여 변형된 제어 로직 정의
- **Implementation (Renderer)**: 구현 클래스들을 위한 인터페이스 정의
- **Concrete Implementation (OpenGLRenderer, DirectXRenderer)**: 실제 구현을 담당

## C 언어 구현

### 구현 인터페이스 정의
```c
// 렌더러 인터페이스 - 구현부 추상화
typedef struct {
    void (*render_circle)(void* self, float x, float y, float radius);
    void (*render_rectangle)(void* self, float x, float y, float width, float height);
    void (*render_line)(void* self, float x1, float y1, float x2, float y2);
    void (*destroy)(void* self);
} Renderer;
```

### 구체적 구현 클래스
```c
// OpenGL 렌더러 구현
typedef struct {
    Renderer interface;
    char* api_name;
} OpenGLRenderer;

void opengl_render_circle(void* self, float x, float y, float radius) {
    OpenGLRenderer* renderer = (OpenGLRenderer*)self;
    printf("[%s] Drawing circle at (%.1f, %.1f) with radius %.1f\n",
           renderer->api_name, x, y, radius);
    printf("  OpenGL: glBegin(GL_TRIANGLE_FAN); /* circle implementation */\n");
}

Renderer* opengl_renderer_create() {
    OpenGLRenderer* renderer = malloc(sizeof(OpenGLRenderer));
    renderer->interface.render_circle = opengl_render_circle;
    renderer->interface.render_rectangle = opengl_render_rectangle;
    renderer->interface.render_line = opengl_render_line;
    renderer->interface.destroy = opengl_destroy;

    renderer->api_name = malloc(strlen("OpenGL") + 1);
    strcpy(renderer->api_name, "OpenGL");

    return (Renderer*)renderer;
}

// DirectX 렌더러 구현
typedef struct {
    Renderer interface;
    char* api_name;
} DirectXRenderer;

void directx_render_circle(void* self, float x, float y, float radius) {
    DirectXRenderer* renderer = (DirectXRenderer*)self;
    printf("[%s] Drawing circle at (%.1f, %.1f) with radius %.1f\n",
           renderer->api_name, x, y, radius);
    printf("  DirectX: DrawIndexedPrimitive(); /* circle implementation */\n");
}
```

### 추상화 인터페이스
```c
// 도형 추상화
typedef struct {
    Renderer* renderer;  // Bridge - 구현부 참조
    void (*draw)(void* self);
    void (*move)(void* self, float dx, float dy);
    void (*resize)(void* self, float factor);
    void (*destroy)(void* self);
} Shape;
```

### 정제된 추상화
```c
// 원 클래스
typedef struct {
    Shape base;
    float x, y, radius;
} Circle;

void circle_draw(void* self) {
    Circle* circle = (Circle*)self;
    // 구현부에 위임
    circle->base.renderer->render_circle(circle->base.renderer,
                                        circle->x, circle->y, circle->radius);
}

void circle_move(void* self, float dx, float dy) {
    Circle* circle = (Circle*)self;
    circle->x += dx;
    circle->y += dy;
    printf("Circle moved by (%.1f, %.1f) to (%.1f, %.1f)\n", dx, dy, circle->x, circle->y);
}

Shape* circle_create(Renderer* renderer, float x, float y, float radius) {
    Circle* circle = malloc(sizeof(Circle));
    circle->base.renderer = renderer;  // Bridge 설정
    circle->base.draw = circle_draw;
    circle->base.move = circle_move;
    circle->base.resize = circle_resize;
    circle->base.destroy = circle_destroy;

    circle->x = x;
    circle->y = y;
    circle->radius = radius;

    return (Shape*)circle;
}
```

## 사용 예제
```c
int main() {
    // 다양한 렌더러 생성
    Renderer* opengl = opengl_renderer_create();
    Renderer* directx = directx_renderer_create();

    // OpenGL로 도형 생성
    Shape* circle_gl = circle_create(opengl, 10.0, 20.0, 5.0);
    Shape* rect_gl = rectangle_create(opengl, 30.0, 40.0, 15.0, 10.0);

    circle_gl->draw(circle_gl);
    rect_gl->draw(rect_gl);

    // DirectX로 도형 생성
    Shape* circle_dx = circle_create(directx, 50.0, 60.0, 8.0);
    Shape* rect_dx = rectangle_create(directx, 70.0, 80.0, 20.0, 12.0);

    circle_dx->draw(circle_dx);
    rect_dx->draw(rect_dx);

    // 도형 조작
    circle_gl->move(circle_gl, 5.0, 3.0);
    circle_gl->draw(circle_gl);

    rect_dx->resize(rect_dx, 1.5);
    rect_dx->draw(rect_dx);

    // 런타임에 렌더러 변경
    Circle* circle_obj = (Circle*)circle_gl;
    circle_obj->base.renderer = directx;
    circle_gl->draw(circle_gl);

    return 0;
}
```

출력 예제:
```
[OpenGL] Drawing circle at (10.0, 20.0) with radius 5.0
  OpenGL: glBegin(GL_TRIANGLE_FAN); /* circle implementation */
[DirectX] Drawing circle at (50.0, 60.0) with radius 8.0
  DirectX: DrawIndexedPrimitive(); /* circle implementation */
Circle moved by (5.0, 3.0) to (15.0, 23.0)
[OpenGL] Drawing circle at (15.0, 23.0) with radius 5.0
Rectangle resized by factor 1.5, new size: 30.0x18.0
[DirectX] Drawing rectangle at (70.0, 80.0) size 30.0x18.0
```

## 장점과 단점

### 장점
- **플랫폼 독립성**: 추상화와 구현을 분리하여 플랫폼별 구현 가능
- **런타임 변경**: 실행 중에 구현을 변경할 수 있음
- **확장성**: 새로운 추상화나 구현을 독립적으로 추가 가능
- **숨겨진 구현 세부사항**: 클라이언트로부터 구현 세부사항을 숨김
- **코드 재사용**: 동일한 추상화를 다양한 구현에서 재사용 가능

### 단점
- **복잡성 증가**: 추가적인 인터페이스와 위임으로 인한 복잡성
- **성능 오버헤드**: 간접 호출로 인한 약간의 성능 저하
- **디자인 복잡도**: 초기 설계 시 더 많은 고려사항 필요

## 적용 상황
- **그래픽 렌더링**: OpenGL, DirectX, Vulkan 등 다양한 그래픽 API 지원
- **데이터베이스 연결**: MySQL, PostgreSQL, SQLite 등 다양한 DB 지원
- **운영체제별 구현**: Windows, Linux, macOS별 네이티브 API 사용
- **네트워크 프로토콜**: TCP, UDP, HTTP 등 다양한 프로토콜 지원
- **UI 프레임워크**: 다양한 플랫폼별 UI 구현 지원
- **장치 드라이버**: 하드웨어별 드라이버 추상화

Bridge 패턴은 특히 크로스 플랫폼 개발이나 다양한 백엔드 구현을 지원해야 하는 상황에서 매우 유용하며, C 언어의 함수 포인터를 활용한 다형성 구현의 좋은 예시입니다.