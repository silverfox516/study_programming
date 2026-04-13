// Liskov Substitution Principle (LSP)
// Subtypes must be substitutable for their base types without altering correctness.

#include <iostream>
#include <string>
#include <cassert>
#include <stdexcept>

// ============================================================
// BAD: Square violates Rectangle's contract
// ============================================================
namespace bad {

class Rectangle {
protected:
    int width_, height_;
public:
    Rectangle(int w, int h) : width_(w), height_(h) {}
    virtual ~Rectangle() = default;

    virtual void set_width(int w) { width_ = w; }
    virtual void set_height(int h) { height_ = h; }
    int width() const { return width_; }
    int height() const { return height_; }
    int area() const { return width_ * height_; }
};

class Square : public Rectangle {
public:
    explicit Square(int side) : Rectangle(side, side) {}

    // VIOLATION: changes both dimensions — breaks Rectangle's postcondition
    void set_width(int w) override { width_ = w; height_ = w; }
    void set_height(int h) override { width_ = h; height_ = h; }
};

// This function assumes Rectangle's contract: setting width doesn't change height
void resize_and_check(Rectangle& r) {
    r.set_width(5);
    r.set_height(10);
    // Expectation: area = 5 * 10 = 50
    std::cout << "  Area = " << r.area()
              << (r.area() == 50 ? " (correct)" : " (WRONG!)") << "\n";
}

} // namespace bad

// ============================================================
// GOOD: Separate types, shared interface for what they actually share
// ============================================================
namespace good {

class Shape {
public:
    virtual ~Shape() = default;
    virtual int area() const = 0;
    virtual std::string name() const = 0;
};

class Rectangle : public Shape {
    int width_, height_;
public:
    Rectangle(int w, int h) : width_(w), height_(h) {}
    void set_width(int w) { width_ = w; }
    void set_height(int h) { height_ = h; }
    int width() const { return width_; }
    int height() const { return height_; }
    int area() const override { return width_ * height_; }
    std::string name() const override { return "Rectangle"; }
};

class Square : public Shape {
    int side_;
public:
    explicit Square(int s) : side_(s) {}
    void set_side(int s) { side_ = s; }
    int side() const { return side_; }
    int area() const override { return side_ * side_; }
    std::string name() const override { return "Square"; }
};

// Works correctly with any Shape — no broken assumptions
void print_area(const Shape& s) {
    std::cout << "  " << s.name() << " area = " << s.area() << "\n";
}

} // namespace good

int main() {
    std::cout << "=== BAD: Square violates Rectangle contract ===\n";
    {
        bad::Rectangle rect(3, 4);
        std::cout << "Rectangle: ";
        bad::resize_and_check(rect);  // correct: 50

        bad::Square sq(3);
        std::cout << "Square:    ";
        bad::resize_and_check(sq);    // WRONG: 100 (set_height overrides width)
    }

    std::cout << "\n=== GOOD: No broken substitution ===\n";
    {
        good::Rectangle rect(5, 10);
        good::Square sq(7);
        good::print_area(rect);  // 50
        good::print_area(sq);    // 49
        // Each type fulfills its own contract correctly
    }

    return 0;
}
