#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265359

typedef enum {
    SHAPE_RESULT_OK = 0,
    SHAPE_RESULT_NULL_PTR,
    SHAPE_RESULT_ALLOC_FAILED,
    SHAPE_RESULT_INVALID_TYPE,
    SHAPE_RESULT_INVALID_PARAM
} ShapeResult;

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

typedef struct Circle {
    Shape base;
    double radius;
} Circle;

typedef struct Rectangle {
    Shape base;
    double width, height;
} Rectangle;

typedef struct Triangle {
    Shape base;
    double base_length, height;
} Triangle;

void circle_draw(Shape* shape) {
    if (!shape) return;
    Circle* circle = (Circle*)shape;
    printf("Drawing Circle with radius %.2f\n", circle->radius);
}

double circle_area(Shape* shape) {
    if (!shape) return 0.0;
    Circle* circle = (Circle*)shape;
    return PI * circle->radius * circle->radius;
}

void circle_destroy(Shape* shape) {
    if (shape) {
        free(shape);
    }
}

void rectangle_draw(Shape* shape) {
    if (!shape) return;
    Rectangle* rect = (Rectangle*)shape;
    printf("Drawing Rectangle %.2fx%.2f\n", rect->width, rect->height);
}

double rectangle_area(Shape* shape) {
    if (!shape) return 0.0;
    Rectangle* rect = (Rectangle*)shape;
    return rect->width * rect->height;
}

void rectangle_destroy(Shape* shape) {
    if (shape) {
        free(shape);
    }
}

void triangle_draw(Shape* shape) {
    if (!shape) return;
    Triangle* tri = (Triangle*)shape;
    printf("Drawing Triangle base:%.2f height:%.2f\n", tri->base_length, tri->height);
}

double triangle_area(Shape* shape) {
    if (!shape) return 0.0;
    Triangle* tri = (Triangle*)shape;
    return 0.5 * tri->base_length * tri->height;
}

void triangle_destroy(Shape* shape) {
    if (shape) {
        free(shape);
    }
}

ShapeResult create_shape(ShapeType type, double param1, double param2, Shape** out_shape) {
    if (!out_shape) {
        return SHAPE_RESULT_NULL_PTR;
    }

    *out_shape = NULL;

    switch (type) {
        case SHAPE_CIRCLE: {
            if (param1 <= 0) {
                return SHAPE_RESULT_INVALID_PARAM;
            }
            Circle* circle = malloc(sizeof(Circle));
            if (!circle) {
                return SHAPE_RESULT_ALLOC_FAILED;
            }
            circle->base.draw = circle_draw;
            circle->base.calculate_area = circle_area;
            circle->base.destroy = circle_destroy;
            circle->radius = param1;
            *out_shape = (Shape*)circle;
            return SHAPE_RESULT_OK;
        }
        case SHAPE_RECTANGLE: {
            if (param1 <= 0 || param2 <= 0) {
                return SHAPE_RESULT_INVALID_PARAM;
            }
            Rectangle* rect = malloc(sizeof(Rectangle));
            if (!rect) {
                return SHAPE_RESULT_ALLOC_FAILED;
            }
            rect->base.draw = rectangle_draw;
            rect->base.calculate_area = rectangle_area;
            rect->base.destroy = rectangle_destroy;
            rect->width = param1;
            rect->height = param2;
            *out_shape = (Shape*)rect;
            return SHAPE_RESULT_OK;
        }
        case SHAPE_TRIANGLE: {
            if (param1 <= 0 || param2 <= 0) {
                return SHAPE_RESULT_INVALID_PARAM;
            }
            Triangle* tri = malloc(sizeof(Triangle));
            if (!tri) {
                return SHAPE_RESULT_ALLOC_FAILED;
            }
            tri->base.draw = triangle_draw;
            tri->base.calculate_area = triangle_area;
            tri->base.destroy = triangle_destroy;
            tri->base_length = param1;
            tri->height = param2;
            *out_shape = (Shape*)tri;
            return SHAPE_RESULT_OK;
        }
        default:
            return SHAPE_RESULT_INVALID_TYPE;
    }
}

const char* shape_result_to_string(ShapeResult result) {
    switch (result) {
        case SHAPE_RESULT_OK: return "Success";
        case SHAPE_RESULT_NULL_PTR: return "Null pointer";
        case SHAPE_RESULT_ALLOC_FAILED: return "Memory allocation failed";
        case SHAPE_RESULT_INVALID_TYPE: return "Invalid shape type";
        case SHAPE_RESULT_INVALID_PARAM: return "Invalid parameter";
        default: return "Unknown error";
    }
}

int main() {
    Shape* shapes[3] = {NULL, NULL, NULL};
    ShapeResult result;

    result = create_shape(SHAPE_CIRCLE, 5.0, 0.0, &shapes[0]);
    if (result != SHAPE_RESULT_OK) {
        printf("Failed to create circle: %s\n", shape_result_to_string(result));
    }

    result = create_shape(SHAPE_RECTANGLE, 4.0, 6.0, &shapes[1]);
    if (result != SHAPE_RESULT_OK) {
        printf("Failed to create rectangle: %s\n", shape_result_to_string(result));
    }

    result = create_shape(SHAPE_TRIANGLE, 3.0, 4.0, &shapes[2]);
    if (result != SHAPE_RESULT_OK) {
        printf("Failed to create triangle: %s\n", shape_result_to_string(result));
    }

    for (int i = 0; i < 3; i++) {
        if (shapes[i]) {
            shapes[i]->draw(shapes[i]);
            printf("Area: %.2f\n\n", shapes[i]->calculate_area(shapes[i]));
            shapes[i]->destroy(shapes[i]);
        }
    }

    return 0;
}