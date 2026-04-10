// Designated init for nested aggregates and arrays — C++20
// What: designated initializers compose into nested aggregates and into arrays
//       (within the array's element-type aggregate).
// Why:  large config-like aggregates become readable when each field is named.
// Pre:  positional braces only, where you had to count commas to find the right slot.

#include <iostream>

struct Color {
    int r;
    int g;
    int b;
};

struct Material {
    Color base;
    Color highlight;
    int   shininess;
};

void show(const Material& m) {
    std::cout << "base       (" << m.base.r      << "," << m.base.g      << "," << m.base.b      << ")\n";
    std::cout << "highlight  (" << m.highlight.r << "," << m.highlight.g << "," << m.highlight.b << ")\n";
    std::cout << "shininess  " << m.shininess << "\n";
}

int main() {
    // Nested designated init — each level addressed by name
    Material m{
        .base      = {.r = 200, .g = 100, .b =  50},
        .highlight = {.r = 255, .g = 200, .b = 150},
        .shininess = 80,
    };
    show(m);

    // Arrays of aggregates can be designated too (per element)
    struct Pair { int a; int b; };
    Pair pairs[3] = {
        {.a = 1, .b = 2},
        {.a = 3, .b = 4},
        {.a = 5, .b = 6},
    };
    for (const auto& p : pairs) {
        std::cout << "pair " << p.a << "," << p.b << "\n";
    }

    return 0;
}
