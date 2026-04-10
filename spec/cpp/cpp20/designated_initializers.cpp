// Designated initializers — C++20
// What: brace-init aggregates by naming the field: Point{.x = 1, .y = 2}.
// Why:  self-documenting, order-independent (within constraints), and safer when
//       a struct gains a new field.
// Pre:  positional braces only — silently scrambled if you got the order wrong.

#include <iostream>
#include <string>

struct Window {
    int         width;
    int         height;
    std::string title;
    bool        fullscreen = false;
};

void show(const Window& w) {
    std::cout << w.title << " " << w.width << "x" << w.height
              << (w.fullscreen ? " [fullscreen]" : "") << "\n";
}

int main() {
    // All fields named — clearest at the call site
    Window w1{.width = 1280, .height = 720, .title = "main"};
    show(w1);

    // Skip a field — it falls back to its default initializer
    Window w2{.width = 1920, .height = 1080, .title = "tv", .fullscreen = true};
    show(w2);

    // Restrictions: designators must appear in declaration order, and you can
    // only initialize aggregates this way (not classes with user-provided ctors).

    return 0;
}
