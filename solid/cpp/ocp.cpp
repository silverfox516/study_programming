// Open/Closed Principle (OCP)
// Software entities should be open for extension, closed for modification.

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>

// ============================================================
// BAD: Adding a new shape requires modifying area_calculator
// ============================================================
namespace bad {

enum class ShapeType { Circle, Rectangle, Triangle };

struct Shape {
    ShapeType type;
    double a, b; // generic dimensions
};

double area_calculator(const Shape& s) {
    switch (s.type) {
        case ShapeType::Circle:    return M_PI * s.a * s.a;
        case ShapeType::Rectangle: return s.a * s.b;
        case ShapeType::Triangle:  return 0.5 * s.a * s.b;
        // Adding a new shape means modifying THIS function
    }
    return 0;
}

} // namespace bad

// ============================================================
// GOOD: Extend via new classes, no existing code modified
// ============================================================
namespace good {

class Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;
    virtual std::string name() const = 0;
};

class Circle : public Shape {
    double radius_;
public:
    explicit Circle(double r) : radius_(r) {}
    double area() const override { return M_PI * radius_ * radius_; }
    std::string name() const override { return "Circle"; }
};

class Rectangle : public Shape {
    double w_, h_;
public:
    Rectangle(double w, double h) : w_(w), h_(h) {}
    double area() const override { return w_ * h_; }
    std::string name() const override { return "Rectangle"; }
};

// EXTENSION: new shape without modifying existing code
class Pentagon : public Shape {
    double side_;
public:
    explicit Pentagon(double s) : side_(s) {}
    double area() const override {
        return 0.25 * std::sqrt(5 * (5 + 2 * std::sqrt(5))) * side_ * side_;
    }
    std::string name() const override { return "Pentagon"; }
};

// This function never changes regardless of new shapes
double total_area(const std::vector<std::unique_ptr<Shape>>& shapes) {
    double sum = 0;
    for (const auto& s : shapes) sum += s->area();
    return sum;
}

} // namespace good

int main() {
    std::cout << "=== BAD: Must modify calculator for each new shape ===\n";
    {
        bad::Shape c{bad::ShapeType::Circle, 5.0, 0};
        bad::Shape r{bad::ShapeType::Rectangle, 3.0, 4.0};
        std::cout << "  Circle area: " << bad::area_calculator(c) << "\n";
        std::cout << "  Rect area:   " << bad::area_calculator(r) << "\n";
    }

    std::cout << "\n=== GOOD: Extend without modifying ===\n";
    {
        std::vector<std::unique_ptr<good::Shape>> shapes;
        shapes.push_back(std::make_unique<good::Circle>(5.0));
        shapes.push_back(std::make_unique<good::Rectangle>(3.0, 4.0));
        shapes.push_back(std::make_unique<good::Pentagon>(3.0));

        for (const auto& s : shapes)
            std::cout << "  " << s->name() << " area: " << s->area() << "\n";
        std::cout << "  Total: " << good::total_area(shapes) << "\n";
    }

    return 0;
}
