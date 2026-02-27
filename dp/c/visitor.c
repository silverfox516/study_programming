#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

// Forward declarations
typedef struct Visitor Visitor;
typedef struct Element Element;

// Element types
typedef enum {
    ELEMENT_CIRCLE,
    ELEMENT_RECTANGLE,
    ELEMENT_TRIANGLE
} ElementType;

// Visitor interface
typedef struct Visitor {
    void (*visit_circle)(struct Visitor* self, void* circle);
    void (*visit_rectangle)(struct Visitor* self, void* rectangle);
    void (*visit_triangle)(struct Visitor* self, void* triangle);
    void (*destroy)(struct Visitor* self);
} Visitor;

// Element interface
typedef struct Element {
    ElementType type;
    void (*accept)(struct Element* self, Visitor* visitor);
    void (*destroy)(struct Element* self);
} Element;

// Concrete Element 1 - Circle
typedef struct {
    Element base;
    float x, y;
    float radius;
} Circle;

void circle_accept(Element* self, Visitor* visitor) {
    visitor->visit_circle(visitor, self);
}

void circle_destroy(Element* self) {
    free(self);
}

Element* circle_create(float x, float y, float radius) {
    Circle* circle = malloc(sizeof(Circle));
    circle->base.type = ELEMENT_CIRCLE;
    circle->base.accept = circle_accept;
    circle->base.destroy = circle_destroy;

    circle->x = x;
    circle->y = y;
    circle->radius = radius;

    return (Element*)circle;
}

// Concrete Element 2 - Rectangle
typedef struct {
    Element base;
    float x, y;
    float width, height;
} Rectangle;

void rectangle_accept(Element* self, Visitor* visitor) {
    visitor->visit_rectangle(visitor, self);
}

void rectangle_destroy(Element* self) {
    free(self);
}

Element* rectangle_create(float x, float y, float width, float height) {
    Rectangle* rect = malloc(sizeof(Rectangle));
    rect->base.type = ELEMENT_RECTANGLE;
    rect->base.accept = rectangle_accept;
    rect->base.destroy = rectangle_destroy;

    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;

    return (Element*)rect;
}

// Concrete Element 3 - Triangle
typedef struct {
    Element base;
    float x1, y1;
    float x2, y2;
    float x3, y3;
} Triangle;

void triangle_accept(Element* self, Visitor* visitor) {
    visitor->visit_triangle(visitor, self);
}

void triangle_destroy(Element* self) {
    free(self);
}

Element* triangle_create(float x1, float y1, float x2, float y2, float x3, float y3) {
    Triangle* triangle = malloc(sizeof(Triangle));
    triangle->base.type = ELEMENT_TRIANGLE;
    triangle->base.accept = triangle_accept;
    triangle->base.destroy = triangle_destroy;

    triangle->x1 = x1;
    triangle->y1 = y1;
    triangle->x2 = x2;
    triangle->y2 = y2;
    triangle->x3 = x3;
    triangle->y3 = y3;

    return (Element*)triangle;
}

// Concrete Visitor 1 - Area Calculator
typedef struct {
    Visitor base;
    float total_area;
} AreaCalculator;

void area_visit_circle(Visitor* self, void* element) {
    AreaCalculator* calc = (AreaCalculator*)self;
    Circle* circle = (Circle*)element;

    float area = M_PI * circle->radius * circle->radius;
    calc->total_area += area;

    printf("Circle at (%.1f, %.1f) with radius %.1f has area: %.2f\n",
           circle->x, circle->y, circle->radius, area);
}

void area_visit_rectangle(Visitor* self, void* element) {
    AreaCalculator* calc = (AreaCalculator*)self;
    Rectangle* rect = (Rectangle*)element;

    float area = rect->width * rect->height;
    calc->total_area += area;

    printf("Rectangle at (%.1f, %.1f) with size %.1fx%.1f has area: %.2f\n",
           rect->x, rect->y, rect->width, rect->height, area);
}

void area_visit_triangle(Visitor* self, void* element) {
    AreaCalculator* calc = (AreaCalculator*)self;
    Triangle* triangle = (Triangle*)element;

    // Using shoelace formula for triangle area
    float area = 0.5 * fabs((triangle->x1 * (triangle->y2 - triangle->y3) +
                            triangle->x2 * (triangle->y3 - triangle->y1) +
                            triangle->x3 * (triangle->y1 - triangle->y2)));
    calc->total_area += area;

    printf("Triangle with vertices (%.1f,%.1f), (%.1f,%.1f), (%.1f,%.1f) has area: %.2f\n",
           triangle->x1, triangle->y1, triangle->x2, triangle->y2,
           triangle->x3, triangle->y3, area);
}

void area_destroy(Visitor* self) {
    free(self);
}

Visitor* area_calculator_create() {
    AreaCalculator* calc = malloc(sizeof(AreaCalculator));
    calc->base.visit_circle = area_visit_circle;
    calc->base.visit_rectangle = area_visit_rectangle;
    calc->base.visit_triangle = area_visit_triangle;
    calc->base.destroy = area_destroy;

    calc->total_area = 0.0;

    return (Visitor*)calc;
}

float area_calculator_get_total(AreaCalculator* calc) {
    return calc->total_area;
}

// Concrete Visitor 2 - Drawing Visitor
typedef struct {
    Visitor base;
    char* output_format;
    int shapes_drawn;
} DrawingVisitor;

void draw_visit_circle(Visitor* self, void* element) {
    DrawingVisitor* drawer = (DrawingVisitor*)self;
    Circle* circle = (Circle*)element;

    printf("[%s] Drawing circle: center(%.1f, %.1f), radius=%.1f\n",
           drawer->output_format, circle->x, circle->y, circle->radius);

    if (strcmp(drawer->output_format, "SVG") == 0) {
        printf("  <circle cx=\"%.1f\" cy=\"%.1f\" r=\"%.1f\" />\n",
               circle->x, circle->y, circle->radius);
    } else if (strcmp(drawer->output_format, "Canvas") == 0) {
        printf("  ctx.arc(%.1f, %.1f, %.1f, 0, 2*Math.PI);\n",
               circle->x, circle->y, circle->radius);
    }

    drawer->shapes_drawn++;
}

void draw_visit_rectangle(Visitor* self, void* element) {
    DrawingVisitor* drawer = (DrawingVisitor*)self;
    Rectangle* rect = (Rectangle*)element;

    printf("[%s] Drawing rectangle: position(%.1f, %.1f), size(%.1fx%.1f)\n",
           drawer->output_format, rect->x, rect->y, rect->width, rect->height);

    if (strcmp(drawer->output_format, "SVG") == 0) {
        printf("  <rect x=\"%.1f\" y=\"%.1f\" width=\"%.1f\" height=\"%.1f\" />\n",
               rect->x, rect->y, rect->width, rect->height);
    } else if (strcmp(drawer->output_format, "Canvas") == 0) {
        printf("  ctx.rect(%.1f, %.1f, %.1f, %.1f);\n",
               rect->x, rect->y, rect->width, rect->height);
    }

    drawer->shapes_drawn++;
}

void draw_visit_triangle(Visitor* self, void* element) {
    DrawingVisitor* drawer = (DrawingVisitor*)self;
    Triangle* triangle = (Triangle*)element;

    printf("[%s] Drawing triangle: vertices(%.1f,%.1f), (%.1f,%.1f), (%.1f,%.1f)\n",
           drawer->output_format, triangle->x1, triangle->y1,
           triangle->x2, triangle->y2, triangle->x3, triangle->y3);

    if (strcmp(drawer->output_format, "SVG") == 0) {
        printf("  <polygon points=\"%.1f,%.1f %.1f,%.1f %.1f,%.1f\" />\n",
               triangle->x1, triangle->y1, triangle->x2, triangle->y2,
               triangle->x3, triangle->y3);
    } else if (strcmp(drawer->output_format, "Canvas") == 0) {
        printf("  ctx.beginPath();\n");
        printf("  ctx.moveTo(%.1f, %.1f);\n", triangle->x1, triangle->y1);
        printf("  ctx.lineTo(%.1f, %.1f);\n", triangle->x2, triangle->y2);
        printf("  ctx.lineTo(%.1f, %.1f);\n", triangle->x3, triangle->y3);
        printf("  ctx.closePath();\n");
    }

    drawer->shapes_drawn++;
}

void draw_destroy(Visitor* self) {
    DrawingVisitor* drawer = (DrawingVisitor*)self;
    if (drawer->output_format) {
        free(drawer->output_format);
    }
    free(drawer);
}

Visitor* drawing_visitor_create(const char* format) {
    DrawingVisitor* drawer = malloc(sizeof(DrawingVisitor));
    drawer->base.visit_circle = draw_visit_circle;
    drawer->base.visit_rectangle = draw_visit_rectangle;
    drawer->base.visit_triangle = draw_visit_triangle;
    drawer->base.destroy = draw_destroy;

    drawer->output_format = malloc(strlen(format) + 1);
    strcpy(drawer->output_format, format);
    drawer->shapes_drawn = 0;

    return (Visitor*)drawer;
}

int drawing_visitor_get_count(DrawingVisitor* drawer) {
    return drawer->shapes_drawn;
}

// Concrete Visitor 3 - Bounds Calculator
typedef struct {
    Visitor base;
    float min_x, min_y;
    float max_x, max_y;
    bool initialized;
} BoundsCalculator;

void bounds_visit_circle(Visitor* self, void* element) {
    BoundsCalculator* bounds = (BoundsCalculator*)self;
    Circle* circle = (Circle*)element;

    float left = circle->x - circle->radius;
    float right = circle->x + circle->radius;
    float top = circle->y - circle->radius;
    float bottom = circle->y + circle->radius;

    if (!bounds->initialized) {
        bounds->min_x = left;
        bounds->max_x = right;
        bounds->min_y = top;
        bounds->max_y = bottom;
        bounds->initialized = true;
    } else {
        if (left < bounds->min_x) bounds->min_x = left;
        if (right > bounds->max_x) bounds->max_x = right;
        if (top < bounds->min_y) bounds->min_y = top;
        if (bottom > bounds->max_y) bounds->max_y = bottom;
    }

    printf("Circle bounds: (%.1f, %.1f) to (%.1f, %.1f)\n", left, top, right, bottom);
}

void bounds_visit_rectangle(Visitor* self, void* element) {
    BoundsCalculator* bounds = (BoundsCalculator*)self;
    Rectangle* rect = (Rectangle*)element;

    float left = rect->x;
    float right = rect->x + rect->width;
    float top = rect->y;
    float bottom = rect->y + rect->height;

    if (!bounds->initialized) {
        bounds->min_x = left;
        bounds->max_x = right;
        bounds->min_y = top;
        bounds->max_y = bottom;
        bounds->initialized = true;
    } else {
        if (left < bounds->min_x) bounds->min_x = left;
        if (right > bounds->max_x) bounds->max_x = right;
        if (top < bounds->min_y) bounds->min_y = top;
        if (bottom > bounds->max_y) bounds->max_y = bottom;
    }

    printf("Rectangle bounds: (%.1f, %.1f) to (%.1f, %.1f)\n", left, top, right, bottom);
}

void bounds_visit_triangle(Visitor* self, void* element) {
    BoundsCalculator* bounds = (BoundsCalculator*)self;
    Triangle* triangle = (Triangle*)element;

    float min_x = triangle->x1;
    float max_x = triangle->x1;
    float min_y = triangle->y1;
    float max_y = triangle->y1;

    // Find min/max x and y from all vertices
    if (triangle->x2 < min_x) min_x = triangle->x2;
    if (triangle->x2 > max_x) max_x = triangle->x2;
    if (triangle->x3 < min_x) min_x = triangle->x3;
    if (triangle->x3 > max_x) max_x = triangle->x3;

    if (triangle->y2 < min_y) min_y = triangle->y2;
    if (triangle->y2 > max_y) max_y = triangle->y2;
    if (triangle->y3 < min_y) min_y = triangle->y3;
    if (triangle->y3 > max_y) max_y = triangle->y3;

    if (!bounds->initialized) {
        bounds->min_x = min_x;
        bounds->max_x = max_x;
        bounds->min_y = min_y;
        bounds->max_y = max_y;
        bounds->initialized = true;
    } else {
        if (min_x < bounds->min_x) bounds->min_x = min_x;
        if (max_x > bounds->max_x) bounds->max_x = max_x;
        if (min_y < bounds->min_y) bounds->min_y = min_y;
        if (max_y > bounds->max_y) bounds->max_y = max_y;
    }

    printf("Triangle bounds: (%.1f, %.1f) to (%.1f, %.1f)\n", min_x, min_y, max_x, max_y);
}

void bounds_destroy(Visitor* self) {
    free(self);
}

Visitor* bounds_calculator_create() {
    BoundsCalculator* bounds = malloc(sizeof(BoundsCalculator));
    bounds->base.visit_circle = bounds_visit_circle;
    bounds->base.visit_rectangle = bounds_visit_rectangle;
    bounds->base.visit_triangle = bounds_visit_triangle;
    bounds->base.destroy = bounds_destroy;

    bounds->min_x = bounds->min_y = 0.0;
    bounds->max_x = bounds->max_y = 0.0;
    bounds->initialized = false;

    return (Visitor*)bounds;
}

void bounds_calculator_get_bounds(BoundsCalculator* bounds, float* min_x, float* min_y,
                                 float* max_x, float* max_y) {
    *min_x = bounds->min_x;
    *min_y = bounds->min_y;
    *max_x = bounds->max_x;
    *max_y = bounds->max_y;
}

// Shape collection
typedef struct {
    Element** shapes;
    int count;
    int capacity;
} ShapeCollection;

ShapeCollection* shape_collection_create() {
    ShapeCollection* collection = malloc(sizeof(ShapeCollection));
    collection->shapes = malloc(4 * sizeof(Element*));
    collection->count = 0;
    collection->capacity = 4;
    return collection;
}

void shape_collection_add(ShapeCollection* collection, Element* shape) {
    if (collection->count >= collection->capacity) {
        collection->capacity *= 2;
        collection->shapes = realloc(collection->shapes, collection->capacity * sizeof(Element*));
    }
    collection->shapes[collection->count++] = shape;
}

void shape_collection_accept(ShapeCollection* collection, Visitor* visitor) {
    for (int i = 0; i < collection->count; i++) {
        collection->shapes[i]->accept(collection->shapes[i], visitor);
    }
}

void shape_collection_destroy(ShapeCollection* collection) {
    for (int i = 0; i < collection->count; i++) {
        collection->shapes[i]->destroy(collection->shapes[i]);
    }
    free(collection->shapes);
    free(collection);
}

// Client code
int main() {
    printf("=== Visitor Pattern Demo - Graphics Processing ===\n\n");

    // Create a collection of shapes
    ShapeCollection* shapes = shape_collection_create();

    shape_collection_add(shapes, circle_create(10.0, 10.0, 5.0));
    shape_collection_add(shapes, rectangle_create(20.0, 15.0, 8.0, 6.0));
    shape_collection_add(shapes, triangle_create(0.0, 0.0, 4.0, 0.0, 2.0, 3.0));
    shape_collection_add(shapes, circle_create(30.0, 25.0, 3.0));

    // Visitor 1: Calculate total area
    printf("=== Area Calculation ===\n");
    AreaCalculator* area_calc = (AreaCalculator*)area_calculator_create();
    shape_collection_accept(shapes, (Visitor*)area_calc);
    printf("Total area of all shapes: %.2f\n\n", area_calculator_get_total(area_calc));

    // Visitor 2: Draw shapes in SVG format
    printf("=== SVG Drawing ===\n");
    DrawingVisitor* svg_drawer = (DrawingVisitor*)drawing_visitor_create("SVG");
    shape_collection_accept(shapes, (Visitor*)svg_drawer);
    printf("Shapes drawn in SVG: %d\n\n", drawing_visitor_get_count(svg_drawer));

    // Visitor 3: Draw shapes in Canvas format
    printf("=== Canvas Drawing ===\n");
    DrawingVisitor* canvas_drawer = (DrawingVisitor*)drawing_visitor_create("Canvas");
    shape_collection_accept(shapes, (Visitor*)canvas_drawer);
    printf("Shapes drawn in Canvas: %d\n\n", drawing_visitor_get_count(canvas_drawer));

    // Visitor 4: Calculate bounding box
    printf("=== Bounds Calculation ===\n");
    BoundsCalculator* bounds_calc = (BoundsCalculator*)bounds_calculator_create();
    shape_collection_accept(shapes, (Visitor*)bounds_calc);

    float min_x, min_y, max_x, max_y;
    bounds_calculator_get_bounds(bounds_calc, &min_x, &min_y, &max_x, &max_y);
    printf("Overall bounding box: (%.1f, %.1f) to (%.1f, %.1f)\n", min_x, min_y, max_x, max_y);
    printf("Bounding box size: %.1f x %.1f\n\n", max_x - min_x, max_y - min_y);

    // Cleanup
    area_calc->base.destroy((Visitor*)area_calc);
    svg_drawer->base.destroy((Visitor*)svg_drawer);
    canvas_drawer->base.destroy((Visitor*)canvas_drawer);
    bounds_calc->base.destroy((Visitor*)bounds_calc);
    shape_collection_destroy(shapes);

    printf("=== Visitor Pattern Benefits ===\n");
    printf("1. Adding new operations is easy (just create new visitor)\n");
    printf("2. Operations are grouped in visitor classes\n");
    printf("3. Can accumulate state during traversal\n");
    printf("4. Can work with heterogeneous object collections\n");

    return 0;
}