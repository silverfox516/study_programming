// C++17: extended aggregate initialization — aggregates can have base classes

#include <iostream>
#include <string>

// Base class
struct Point {
    int x, y;
};

// Derived aggregate (C++17 allows brace-init for base class members)
struct NamedPoint : Point {
    std::string name;
};

// Multiple bases
struct Color {
    int r, g, b;
};

struct ColoredPoint : Point, Color {
    float alpha;
};

// Empty base optimization with aggregates
struct Tag {};
struct TaggedValue : Tag {
    int value;
};

int main() {
    // C++17: aggregate init with base class
    NamedPoint np{{10, 20}, "origin"};
    std::cout << "NamedPoint: (" << np.x << ", " << np.y
              << ") name=" << np.name << "\n";

    // Multiple base classes
    ColoredPoint cp{{5, 10}, {255, 0, 128}, 0.75f};
    std::cout << "ColoredPoint: (" << cp.x << ", " << cp.y
              << ") rgb=(" << cp.r << "," << cp.g << "," << cp.b
              << ") alpha=" << cp.alpha << "\n";

    // Empty base
    TaggedValue tv{{}, 42};
    std::cout << "TaggedValue: " << tv.value << "\n";

    // Structured binding works with aggregates
    auto& [x, y] = static_cast<Point&>(np);
    std::cout << "Destructured: x=" << x << ", y=" << y << "\n";

    return 0;
}
