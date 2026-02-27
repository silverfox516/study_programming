#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Implementation interface (Renderer)
typedef struct {
    void (*render_circle)(void* self, float x, float y, float radius);
    void (*render_rectangle)(void* self, float x, float y, float width, float height);
    void (*render_line)(void* self, float x1, float y1, float x2, float y2);
    void (*destroy)(void* self);
} Renderer;

// Concrete Implementation 1 - OpenGL Renderer
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

void opengl_render_rectangle(void* self, float x, float y, float width, float height) {
    OpenGLRenderer* renderer = (OpenGLRenderer*)self;
    printf("[%s] Drawing rectangle at (%.1f, %.1f) size %.1fx%.1f\n",
           renderer->api_name, x, y, width, height);
    printf("  OpenGL: glBegin(GL_QUADS); /* rectangle implementation */\n");
}

void opengl_render_line(void* self, float x1, float y1, float x2, float y2) {
    OpenGLRenderer* renderer = (OpenGLRenderer*)self;
    printf("[%s] Drawing line from (%.1f, %.1f) to (%.1f, %.1f)\n",
           renderer->api_name, x1, y1, x2, y2);
    printf("  OpenGL: glBegin(GL_LINES); /* line implementation */\n");
}

void opengl_destroy(void* self) {
    OpenGLRenderer* renderer = (OpenGLRenderer*)self;
    if (renderer->api_name) {
        free(renderer->api_name);
    }
    free(renderer);
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

// Concrete Implementation 2 - DirectX Renderer
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

void directx_render_rectangle(void* self, float x, float y, float width, float height) {
    DirectXRenderer* renderer = (DirectXRenderer*)self;
    printf("[%s] Drawing rectangle at (%.1f, %.1f) size %.1fx%.1f\n",
           renderer->api_name, x, y, width, height);
    printf("  DirectX: DrawPrimitive(); /* rectangle implementation */\n");
}

void directx_render_line(void* self, float x1, float y1, float x2, float y2) {
    DirectXRenderer* renderer = (DirectXRenderer*)self;
    printf("[%s] Drawing line from (%.1f, %.1f) to (%.1f, %.1f)\n",
           renderer->api_name, x1, y1, x2, y2);
    printf("  DirectX: DrawPrimitiveUP(); /* line implementation */\n");
}

void directx_destroy(void* self) {
    DirectXRenderer* renderer = (DirectXRenderer*)self;
    if (renderer->api_name) {
        free(renderer->api_name);
    }
    free(renderer);
}

Renderer* directx_renderer_create() {
    DirectXRenderer* renderer = malloc(sizeof(DirectXRenderer));
    renderer->interface.render_circle = directx_render_circle;
    renderer->interface.render_rectangle = directx_render_rectangle;
    renderer->interface.render_line = directx_render_line;
    renderer->interface.destroy = directx_destroy;

    renderer->api_name = malloc(strlen("DirectX") + 1);
    strcpy(renderer->api_name, "DirectX");

    return (Renderer*)renderer;
}

// Abstraction - Shape
typedef struct {
    Renderer* renderer;
    void (*draw)(void* self);
    void (*move)(void* self, float dx, float dy);
    void (*resize)(void* self, float factor);
    void (*destroy)(void* self);
} Shape;

// Refined Abstraction 1 - Circle
typedef struct {
    Shape base;
    float x, y, radius;
} Circle;

void circle_draw(void* self) {
    Circle* circle = (Circle*)self;
    circle->base.renderer->render_circle(circle->base.renderer,
                                        circle->x, circle->y, circle->radius);
}

void circle_move(void* self, float dx, float dy) {
    Circle* circle = (Circle*)self;
    circle->x += dx;
    circle->y += dy;
    printf("Circle moved by (%.1f, %.1f) to (%.1f, %.1f)\n", dx, dy, circle->x, circle->y);
}

void circle_resize(void* self, float factor) {
    Circle* circle = (Circle*)self;
    circle->radius *= factor;
    printf("Circle resized by factor %.1f, new radius: %.1f\n", factor, circle->radius);
}

void circle_destroy(void* self) {
    free(self);
}

Shape* circle_create(Renderer* renderer, float x, float y, float radius) {
    Circle* circle = malloc(sizeof(Circle));
    circle->base.renderer = renderer;
    circle->base.draw = circle_draw;
    circle->base.move = circle_move;
    circle->base.resize = circle_resize;
    circle->base.destroy = circle_destroy;

    circle->x = x;
    circle->y = y;
    circle->radius = radius;

    return (Shape*)circle;
}

// Refined Abstraction 2 - Rectangle
typedef struct {
    Shape base;
    float x, y, width, height;
} Rectangle;

void rectangle_draw(void* self) {
    Rectangle* rect = (Rectangle*)self;
    rect->base.renderer->render_rectangle(rect->base.renderer,
                                         rect->x, rect->y, rect->width, rect->height);
}

void rectangle_move(void* self, float dx, float dy) {
    Rectangle* rect = (Rectangle*)self;
    rect->x += dx;
    rect->y += dy;
    printf("Rectangle moved by (%.1f, %.1f) to (%.1f, %.1f)\n", dx, dy, rect->x, rect->y);
}

void rectangle_resize(void* self, float factor) {
    Rectangle* rect = (Rectangle*)self;
    rect->width *= factor;
    rect->height *= factor;
    printf("Rectangle resized by factor %.1f, new size: %.1fx%.1f\n",
           factor, rect->width, rect->height);
}

void rectangle_destroy(void* self) {
    free(self);
}

Shape* rectangle_create(Renderer* renderer, float x, float y, float width, float height) {
    Rectangle* rect = malloc(sizeof(Rectangle));
    rect->base.renderer = renderer;
    rect->base.draw = rectangle_draw;
    rect->base.move = rectangle_move;
    rect->base.resize = rectangle_resize;
    rect->base.destroy = rectangle_destroy;

    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;

    return (Shape*)rect;
}

// Client code
int main() {
    printf("=== Bridge Pattern Demo - Graphics Rendering ===\n\n");

    // Create different renderers
    Renderer* opengl = opengl_renderer_create();
    Renderer* directx = directx_renderer_create();

    printf("1. Creating shapes with OpenGL renderer:\n");
    Shape* circle_gl = circle_create(opengl, 10.0, 20.0, 5.0);
    Shape* rect_gl = rectangle_create(opengl, 30.0, 40.0, 15.0, 10.0);

    circle_gl->draw(circle_gl);
    rect_gl->draw(rect_gl);

    printf("\n2. Creating shapes with DirectX renderer:\n");
    Shape* circle_dx = circle_create(directx, 50.0, 60.0, 8.0);
    Shape* rect_dx = rectangle_create(directx, 70.0, 80.0, 20.0, 12.0);

    circle_dx->draw(circle_dx);
    rect_dx->draw(rect_dx);

    printf("\n3. Moving and resizing shapes:\n");
    circle_gl->move(circle_gl, 5.0, 3.0);
    circle_gl->draw(circle_gl);

    rect_dx->resize(rect_dx, 1.5);
    rect_dx->draw(rect_dx);

    printf("\n4. Switching renderer for existing shape:\n");
    printf("Switching circle to DirectX renderer:\n");
    // Simulate changing renderer (in real implementation, this might involve more complex logic)
    Circle* circle_obj = (Circle*)circle_gl;
    circle_obj->base.renderer = directx;
    circle_gl->draw(circle_gl);

    // Cleanup
    circle_gl->destroy(circle_gl);
    rect_gl->destroy(rect_gl);
    circle_dx->destroy(circle_dx);
    rect_dx->destroy(rect_dx);

    opengl->destroy(opengl);
    directx->destroy(directx);

    return 0;
}