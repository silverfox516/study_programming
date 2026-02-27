#include <stdio.h>
#include <stdlib.h>

// Abstract products
typedef struct Button {
    void (*paint)(struct Button* self);
    void (*destroy)(struct Button* self);
} Button;

typedef struct Checkbox {
    void (*paint)(struct Checkbox* self);
    void (*destroy)(struct Checkbox* self);
} Checkbox;

// Windows implementations
typedef struct WindowsButton {
    Button base;
} WindowsButton;

void windows_button_paint(Button* button) {
    printf("Rendering Windows Button\n");
}

void windows_button_destroy(Button* button) {
    free(button);
}

Button* create_windows_button() {
    WindowsButton* button = malloc(sizeof(WindowsButton));
    button->base.paint = windows_button_paint;
    button->base.destroy = windows_button_destroy;
    return (Button*)button;
}

typedef struct WindowsCheckbox {
    Checkbox base;
} WindowsCheckbox;

void windows_checkbox_paint(Checkbox* checkbox) {
    printf("Rendering Windows Checkbox\n");
}

void windows_checkbox_destroy(Checkbox* checkbox) {
    free(checkbox);
}

Checkbox* create_windows_checkbox() {
    WindowsCheckbox* checkbox = malloc(sizeof(WindowsCheckbox));
    checkbox->base.paint = windows_checkbox_paint;
    checkbox->base.destroy = windows_checkbox_destroy;
    return (Checkbox*)checkbox;
}

// Mac implementations
typedef struct MacButton {
    Button base;
} MacButton;

void mac_button_paint(Button* button) {
    printf("Rendering Mac Button\n");
}

void mac_button_destroy(Button* button) {
    free(button);
}

Button* create_mac_button() {
    MacButton* button = malloc(sizeof(MacButton));
    button->base.paint = mac_button_paint;
    button->base.destroy = mac_button_destroy;
    return (Button*)button;
}

typedef struct MacCheckbox {
    Checkbox base;
} MacCheckbox;

void mac_checkbox_paint(Checkbox* checkbox) {
    printf("Rendering Mac Checkbox\n");
}

void mac_checkbox_destroy(Checkbox* checkbox) {
    free(checkbox);
}

Checkbox* create_mac_checkbox() {
    MacCheckbox* checkbox = malloc(sizeof(MacCheckbox));
    checkbox->base.paint = mac_checkbox_paint;
    checkbox->base.destroy = mac_checkbox_destroy;
    return (Checkbox*)checkbox;
}

// Abstract factory
typedef struct GUIFactory {
    Button* (*create_button)(struct GUIFactory* self);
    Checkbox* (*create_checkbox)(struct GUIFactory* self);
    void (*destroy)(struct GUIFactory* self);
} GUIFactory;

// Windows factory
typedef struct WindowsFactory {
    GUIFactory base;
} WindowsFactory;

Button* windows_factory_create_button(GUIFactory* factory) {
    return create_windows_button();
}

Checkbox* windows_factory_create_checkbox(GUIFactory* factory) {
    return create_windows_checkbox();
}

void windows_factory_destroy(GUIFactory* factory) {
    free(factory);
}

GUIFactory* create_windows_factory() {
    WindowsFactory* factory = malloc(sizeof(WindowsFactory));
    factory->base.create_button = windows_factory_create_button;
    factory->base.create_checkbox = windows_factory_create_checkbox;
    factory->base.destroy = windows_factory_destroy;
    return (GUIFactory*)factory;
}

// Mac factory
typedef struct MacFactory {
    GUIFactory base;
} MacFactory;

Button* mac_factory_create_button(GUIFactory* factory) {
    return create_mac_button();
}

Checkbox* mac_factory_create_checkbox(GUIFactory* factory) {
    return create_mac_checkbox();
}

void mac_factory_destroy(GUIFactory* factory) {
    free(factory);
}

GUIFactory* create_mac_factory() {
    MacFactory* factory = malloc(sizeof(MacFactory));
    factory->base.create_button = mac_factory_create_button;
    factory->base.create_checkbox = mac_factory_create_checkbox;
    factory->base.destroy = mac_factory_destroy;
    return (GUIFactory*)factory;
}

// Application
typedef struct Application {
    GUIFactory* factory;
    Button* button;
    Checkbox* checkbox;
} Application;

Application* create_application(GUIFactory* factory) {
    Application* app = malloc(sizeof(Application));
    app->factory = factory;
    app->button = factory->create_button(factory);
    app->checkbox = factory->create_checkbox(factory);
    return app;
}

void render_ui(Application* app) {
    app->button->paint(app->button);
    app->checkbox->paint(app->checkbox);
}

void destroy_application(Application* app) {
    if (app) {
        if (app->button) app->button->destroy(app->button);
        if (app->checkbox) app->checkbox->destroy(app->checkbox);
        if (app->factory) app->factory->destroy(app->factory);
        free(app);
    }
}

int main() {
    printf("--- GUI Abstract Factory ---\n\n");

    printf("Creating Windows Application:\n");
    GUIFactory* windows_factory = create_windows_factory();
    Application* windows_app = create_application(windows_factory);
    render_ui(windows_app);
    destroy_application(windows_app);

    printf("\nCreating Mac Application:\n");
    GUIFactory* mac_factory = create_mac_factory();
    Application* mac_app = create_application(mac_factory);
    render_ui(mac_app);
    destroy_application(mac_app);

    return 0;
}