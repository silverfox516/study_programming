// Type Erasure Pattern in C
// Uniform interface via function pointer table (vtable)

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    void (*draw)(const void* self);
    double (*area)(const void* self);
} ShapeVTable;

typedef struct { const ShapeVTable* vt; } Shape;

// Circle
typedef struct { Shape base; double radius; } Circle;

void circle_draw(const void* self) {
    const Circle* c = (const Circle*)self;
    printf("  Drawing circle (r=%.1f)\n", c->radius);
}
double circle_area(const void* self) {
    const Circle* c = (const Circle*)self;
    return M_PI * c->radius * c->radius;
}
static const ShapeVTable circle_vt = {circle_draw, circle_area};

Circle circle_new(double r) {
    Circle c;
    c.base.vt = &circle_vt;
    c.radius = r;
    return c;
}

// Rectangle
typedef struct { Shape base; double w, h; } Rect;

void rect_draw(const void* self) {
    const Rect* r = (const Rect*)self;
    printf("  Drawing rect (%.1fx%.1f)\n", r->w, r->h);
}
double rect_area(const void* self) {
    const Rect* r = (const Rect*)self;
    return r->w * r->h;
}
static const ShapeVTable rect_vt = {rect_draw, rect_area};

Rect rect_new(double w, double h) {
    Rect r;
    r.base.vt = &rect_vt;
    r.w = w;
    r.h = h;
    return r;
}

int main(void) {
    Circle c = circle_new(5.0);
    Rect r = rect_new(3.0, 4.0);

    // Type-erased array
    Shape* shapes[] = {(Shape*)&c, (Shape*)&r};
    int n = 2;

    double total = 0;
    for (int i = 0; i < n; i++) {
        shapes[i]->vt->draw(shapes[i]);
        total += shapes[i]->vt->area(shapes[i]);
    }
    printf("Total area: %.2f\n", total);
    return 0;
}
