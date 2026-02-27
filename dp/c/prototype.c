#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Shape {
    char* type;
    int x, y;
    char* color;
    struct Shape* (*clone)(struct Shape* self);
    void (*draw)(struct Shape* self);
    void (*destroy)(struct Shape* self);
} Shape;

typedef struct Rectangle {
    Shape base;
    int width, height;
} Rectangle;

typedef struct Circle {
    Shape base;
    int radius;
} Circle;

Shape* rectangle_clone(Shape* self) {
    Rectangle* original = (Rectangle*)self;
    Rectangle* clone = malloc(sizeof(Rectangle));

    // Copy base shape data
    clone->base.type = malloc(strlen(original->base.type) + 1);
    strcpy(clone->base.type, original->base.type);
    clone->base.color = malloc(strlen(original->base.color) + 1);
    strcpy(clone->base.color, original->base.color);
    clone->base.x = original->base.x;
    clone->base.y = original->base.y;

    // Copy rectangle specific data
    clone->width = original->width;
    clone->height = original->height;

    // Copy function pointers
    clone->base.clone = rectangle_clone;
    clone->base.draw = self->draw;
    clone->base.destroy = self->destroy;

    return (Shape*)clone;
}

void rectangle_draw(Shape* self) {
    Rectangle* rect = (Rectangle*)self;
    printf("Drawing %s Rectangle at (%d,%d) with size %dx%d, color: %s\n",
           rect->base.type, rect->base.x, rect->base.y,
           rect->width, rect->height, rect->base.color);
}

void rectangle_destroy(Shape* self) {
    if (self) {
        free(self->type);
        free(self->color);
        free(self);
    }
}

Shape* create_rectangle(int x, int y, int width, int height, const char* color) {
    Rectangle* rect = malloc(sizeof(Rectangle));

    rect->base.type = malloc(strlen("Rectangle") + 1);
    strcpy(rect->base.type, "Rectangle");

    rect->base.color = malloc(strlen(color) + 1);
    strcpy(rect->base.color, color);

    rect->base.x = x;
    rect->base.y = y;
    rect->width = width;
    rect->height = height;

    rect->base.clone = rectangle_clone;
    rect->base.draw = rectangle_draw;
    rect->base.destroy = rectangle_destroy;

    return (Shape*)rect;
}

Shape* circle_clone(Shape* self) {
    Circle* original = (Circle*)self;
    Circle* clone = malloc(sizeof(Circle));

    // Copy base shape data
    clone->base.type = malloc(strlen(original->base.type) + 1);
    strcpy(clone->base.type, original->base.type);
    clone->base.color = malloc(strlen(original->base.color) + 1);
    strcpy(clone->base.color, original->base.color);
    clone->base.x = original->base.x;
    clone->base.y = original->base.y;

    // Copy circle specific data
    clone->radius = original->radius;

    // Copy function pointers
    clone->base.clone = circle_clone;
    clone->base.draw = self->draw;
    clone->base.destroy = self->destroy;

    return (Shape*)clone;
}

void circle_draw(Shape* self) {
    Circle* circle = (Circle*)self;
    printf("Drawing %s Circle at (%d,%d) with radius %d, color: %s\n",
           circle->base.type, circle->base.x, circle->base.y,
           circle->radius, circle->base.color);
}

void circle_destroy(Shape* self) {
    if (self) {
        free(self->type);
        free(self->color);
        free(self);
    }
}

Shape* create_circle(int x, int y, int radius, const char* color) {
    Circle* circle = malloc(sizeof(Circle));

    circle->base.type = malloc(strlen("Circle") + 1);
    strcpy(circle->base.type, "Circle");

    circle->base.color = malloc(strlen(color) + 1);
    strcpy(circle->base.color, color);

    circle->base.x = x;
    circle->base.y = y;
    circle->radius = radius;

    circle->base.clone = circle_clone;
    circle->base.draw = circle_draw;
    circle->base.destroy = circle_destroy;

    return (Shape*)circle;
}

// Shape registry for prototype management
#define MAX_PROTOTYPES 10

typedef struct ShapeRegistry {
    Shape* prototypes[MAX_PROTOTYPES];
    char* names[MAX_PROTOTYPES];
    int count;
} ShapeRegistry;

ShapeRegistry* create_shape_registry() {
    ShapeRegistry* registry = malloc(sizeof(ShapeRegistry));
    registry->count = 0;
    return registry;
}

void register_shape(ShapeRegistry* registry, const char* name, Shape* prototype) {
    if (registry->count < MAX_PROTOTYPES) {
        registry->names[registry->count] = malloc(strlen(name) + 1);
        strcpy(registry->names[registry->count], name);
        registry->prototypes[registry->count] = prototype;
        registry->count++;
        printf("Registered prototype: %s\n", name);
    }
}

Shape* get_shape(ShapeRegistry* registry, const char* name) {
    for (int i = 0; i < registry->count; i++) {
        if (strcmp(registry->names[i], name) == 0) {
            return registry->prototypes[i]->clone(registry->prototypes[i]);
        }
    }
    return NULL;
}

void destroy_shape_registry(ShapeRegistry* registry) {
    if (registry) {
        for (int i = 0; i < registry->count; i++) {
            free(registry->names[i]);
            registry->prototypes[i]->destroy(registry->prototypes[i]);
        }
        free(registry);
    }
}

int main() {
    printf("--- Prototype Pattern Example ---\n\n");

    // Create registry and register prototypes
    ShapeRegistry* registry = create_shape_registry();

    Shape* red_rectangle = create_rectangle(0, 0, 100, 50, "Red");
    Shape* blue_circle = create_circle(0, 0, 25, "Blue");

    register_shape(registry, "RedRectangle", red_rectangle);
    register_shape(registry, "BlueCircle", blue_circle);

    printf("\n--- Creating shapes from prototypes ---\n");

    // Create shapes by cloning prototypes
    Shape* shape1 = get_shape(registry, "RedRectangle");
    if (shape1) {
        shape1->x = 10;
        shape1->y = 20;
        shape1->draw(shape1);
    }

    Shape* shape2 = get_shape(registry, "BlueCircle");
    if (shape2) {
        shape2->x = 50;
        shape2->y = 75;
        shape2->draw(shape2);
    }

    Shape* shape3 = get_shape(registry, "RedRectangle");
    if (shape3) {
        shape3->x = 100;
        shape3->y = 200;
        shape3->draw(shape3);
    }

    printf("\n--- Original prototypes remain unchanged ---\n");
    red_rectangle->draw(red_rectangle);
    blue_circle->draw(blue_circle);

    // Cleanup
    if (shape1) shape1->destroy(shape1);
    if (shape2) shape2->destroy(shape2);
    if (shape3) shape3->destroy(shape3);
    destroy_shape_registry(registry);

    return 0;
}