// Type Erasure — C++ Idiom
//
// Intent: store objects of any type that satisfies a set of requirements
//         in a single, uniform wrapper — without requiring a common base class.
//         std::function, std::any, and std::move_only_function all use this.
//
// When to use:
//   - You want polymorphism without inheritance (duck typing)
//   - Heterogeneous container of unrelated types with a shared interface
//   - The interface is small (1–3 methods)
//
// When NOT to use:
//   - Performance-critical hot path — heap alloc + virtual dispatch per call
//   - The set of types is known at compile time — use std::variant instead

#include <iostream>
#include <memory>
#include <vector>
#include <string>

// ============================================================================
// A type-erased "Drawable": anything with a draw() method
// ============================================================================
class Drawable {
public:
    // Accept anything with a draw() method — no base class needed
    template <typename T>
    Drawable(T obj)
        : self_(std::make_unique<Model<T>>(std::move(obj))) {}

    Drawable(const Drawable& other) : self_(other.self_->clone()) {}
    Drawable(Drawable&&) noexcept = default;
    Drawable& operator=(Drawable other) {
        self_ = std::move(other.self_);
        return *this;
    }

    void draw() const { self_->draw(); }

private:
    // Concept (internal interface)
    struct Concept {
        virtual ~Concept() = default;
        virtual void draw() const = 0;
        virtual std::unique_ptr<Concept> clone() const = 0;
    };

    // Model wraps any concrete type that has draw()
    template <typename T>
    struct Model : Concept {
        T data;
        explicit Model(T d) : data(std::move(d)) {}
        void draw() const override { data.draw(); }
        std::unique_ptr<Concept> clone() const override {
            return std::make_unique<Model>(data);
        }
    };

    std::unique_ptr<Concept> self_;
};

// ============================================================================
// Concrete types — NO common base class
// ============================================================================
struct Circle {
    double radius;
    void draw() const { std::cout << "  Circle(r=" << radius << ")\n"; }
};

struct Rectangle {
    double w, h;
    void draw() const { std::cout << "  Rect(" << w << "x" << h << ")\n"; }
};

struct Text {
    std::string content;
    void draw() const { std::cout << "  Text(\"" << content << "\")\n"; }
};

// ============================================================================
// Demo: heterogeneous container with value semantics
// ============================================================================
int main() {
    std::cout << "=== Type Erasure Pattern ===\n";

    // All different types, no inheritance — stored in a single vector
    std::vector<Drawable> scene;
    scene.push_back(Circle{5.0});
    scene.push_back(Rectangle{3.0, 4.0});
    scene.push_back(Text{"hello"});

    std::cout << "draw scene:\n";
    for (const auto& d : scene) d.draw();

    // Value semantics — copying a Drawable deep-copies the wrapped object
    auto copy = scene[0];
    std::cout << "copy:\n";
    copy.draw();

    std::cout << "\nKey points:\n";
    std::cout << " * Any type with draw() can be stored — no base class required\n";
    std::cout << " * Concept/Model pair: Concept = interface, Model<T> = adapter\n";
    std::cout << " * Value semantics (copyable) via clone() in Concept\n";
    std::cout << " * This is exactly how std::function works internally\n";
    std::cout << " * Trade-off: heap allocation per wrapper + virtual dispatch\n";
    return 0;
}
