#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Component interface
typedef struct Component {
    char* name;
    void (*draw)(struct Component* self, int indent);
    void (*add)(struct Component* self, struct Component* child);
    void (*remove)(struct Component* self, struct Component* child);
    void (*destroy)(struct Component* self);
} Component;

// Leaf - Simple graphic element
typedef struct {
    Component base;
} Leaf;

void leaf_draw(Component* self, int indent) {
    for (int i = 0; i < indent; i++) printf("  ");
    printf("- %s (Leaf)\n", self->name);
}

void leaf_add(Component* self, Component* child) {
    printf("Cannot add child to leaf node: %s\n", self->name);
}

void leaf_remove(Component* self, Component* child) {
    printf("Cannot remove child from leaf node: %s\n", self->name);
}

void leaf_destroy(Component* self) {
    if (self->name) {
        free(self->name);
    }
    free(self);
}

Component* leaf_create(const char* name) {
    Leaf* leaf = malloc(sizeof(Leaf));
    leaf->base.name = malloc(strlen(name) + 1);
    strcpy(leaf->base.name, name);

    leaf->base.draw = leaf_draw;
    leaf->base.add = leaf_add;
    leaf->base.remove = leaf_remove;
    leaf->base.destroy = leaf_destroy;

    return (Component*)leaf;
}

// Composite - Container that can hold other components
typedef struct {
    Component base;
    Component** children;
    int child_count;
    int capacity;
} Composite;

void composite_draw(Component* self, int indent) {
    Composite* comp = (Composite*)self;

    for (int i = 0; i < indent; i++) printf("  ");
    printf("+ %s (Composite)\n", self->name);

    for (int i = 0; i < comp->child_count; i++) {
        comp->children[i]->draw(comp->children[i], indent + 1);
    }
}

void composite_add(Component* self, Component* child) {
    Composite* comp = (Composite*)self;

    if (comp->child_count >= comp->capacity) {
        comp->capacity *= 2;
        comp->children = realloc(comp->children, comp->capacity * sizeof(Component*));
    }

    comp->children[comp->child_count++] = child;
    printf("Added '%s' to composite '%s'\n", child->name, self->name);
}

void composite_remove(Component* self, Component* child) {
    Composite* comp = (Composite*)self;

    for (int i = 0; i < comp->child_count; i++) {
        if (comp->children[i] == child) {
            // Shift remaining elements
            for (int j = i; j < comp->child_count - 1; j++) {
                comp->children[j] = comp->children[j + 1];
            }
            comp->child_count--;
            printf("Removed '%s' from composite '%s'\n", child->name, self->name);
            return;
        }
    }
    printf("Child '%s' not found in composite '%s'\n", child->name, self->name);
}

void composite_destroy(Component* self) {
    Composite* comp = (Composite*)self;

    // Destroy all children
    for (int i = 0; i < comp->child_count; i++) {
        comp->children[i]->destroy(comp->children[i]);
    }

    if (comp->children) {
        free(comp->children);
    }
    if (self->name) {
        free(self->name);
    }
    free(comp);
}

Component* composite_create(const char* name) {
    Composite* comp = malloc(sizeof(Composite));
    comp->base.name = malloc(strlen(name) + 1);
    strcpy(comp->base.name, name);

    comp->base.draw = composite_draw;
    comp->base.add = composite_add;
    comp->base.remove = composite_remove;
    comp->base.destroy = composite_destroy;

    comp->children = malloc(4 * sizeof(Component*));
    comp->child_count = 0;
    comp->capacity = 4;

    return (Component*)comp;
}

// File system example
int main() {
    printf("=== Composite Pattern Demo - File System ===\n\n");

    // Create root directory
    Component* root = composite_create("root");

    // Create subdirectories
    Component* documents = composite_create("Documents");
    Component* pictures = composite_create("Pictures");
    Component* work = composite_create("Work");

    // Create files (leaves)
    Component* resume = leaf_create("resume.pdf");
    Component* photo1 = leaf_create("vacation.jpg");
    Component* photo2 = leaf_create("family.png");
    Component* project = leaf_create("project.docx");
    Component* report = leaf_create("report.xlsx");

    printf("Building file system structure:\n");

    // Build the structure
    root->add(root, documents);
    root->add(root, pictures);

    documents->add(documents, resume);
    documents->add(documents, work);

    pictures->add(pictures, photo1);
    pictures->add(pictures, photo2);

    work->add(work, project);
    work->add(work, report);

    printf("\nFile system structure:\n");
    root->draw(root, 0);

    printf("\nRemoving vacation.jpg from Pictures:\n");
    pictures->remove(pictures, photo1);

    printf("\nUpdated file system structure:\n");
    root->draw(root, 0);

    // Create a new graphics example
    printf("\n\n=== Graphics Example ===\n");
    Component* canvas = composite_create("Canvas");
    Component* shape_group = composite_create("ShapeGroup");

    Component* circle = leaf_create("Circle");
    Component* rectangle = leaf_create("Rectangle");
    Component* line = leaf_create("Line");
    Component* triangle = leaf_create("Triangle");

    printf("\nBuilding graphics structure:\n");
    canvas->add(canvas, shape_group);
    canvas->add(canvas, line);

    shape_group->add(shape_group, circle);
    shape_group->add(shape_group, rectangle);
    shape_group->add(shape_group, triangle);

    printf("\nGraphics structure:\n");
    canvas->draw(canvas, 0);

    // Cleanup
    printf("\nCleaning up file system...\n");
    photo1->destroy(photo1); // This was removed from pictures but still needs cleanup
    root->destroy(root);

    printf("Cleaning up graphics...\n");
    canvas->destroy(canvas);

    return 0;
}