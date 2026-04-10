// Pimpl (Pointer to Implementation) — C++ Idiom
//
// Intent: hide implementation details behind an opaque pointer so that
//         changes to the implementation do not trigger recompilation of
//         clients that include the header.
//
// When to use:
//   - Large projects where compile-time firewall is valuable
//   - You want to hide third-party headers from your public API
//   - ABI stability across library versions
//
// When NOT to use:
//   - Hot-path types where the indirection cost matters
//   - Simple types with trivial implementation

#include <iostream>
#include <memory>
#include <string>

// ============================================================================
// In real code this would be the .hpp — the public header
// ============================================================================
class Widget {
public:
    Widget(const std::string& name, int value);
    ~Widget();
    Widget(Widget&&) noexcept;
    Widget& operator=(Widget&&) noexcept;

    void do_work() const;
    const std::string& name() const;

private:
    struct Impl;                     // forward declaration only
    std::unique_ptr<Impl> pimpl_;   // opaque pointer
};

// ============================================================================
// In real code this would be the .cpp — the implementation file
// ============================================================================
struct Widget::Impl {
    std::string name;
    int         value;
    int         internal_counter = 0;

    void heavy_computation() {
        ++internal_counter;
        std::cout << "  [impl] heavy work #" << internal_counter
                  << " on " << name << " (value=" << value << ")\n";
    }
};

Widget::Widget(const std::string& name, int value)
    : pimpl_(std::make_unique<Impl>()) {
    pimpl_->name  = name;
    pimpl_->value = value;
}

Widget::~Widget() = default;
Widget::Widget(Widget&&) noexcept = default;
Widget& Widget::operator=(Widget&&) noexcept = default;

void Widget::do_work() const {
    pimpl_->heavy_computation();
}

const std::string& Widget::name() const {
    return pimpl_->name;
}

// ============================================================================
// Demo
// ============================================================================
int main() {
    std::cout << "=== Pimpl Pattern ===\n";

    Widget w("alpha", 42);
    w.do_work();
    w.do_work();
    std::cout << "name: " << w.name() << "\n";

    // Move works
    Widget w2 = std::move(w);
    w2.do_work();

    std::cout << "\nKey points:\n";
    std::cout << " * Header exposes only forward-declared Impl — no details leaked\n";
    std::cout << " * Changing Impl internals does not recompile clients\n";
    std::cout << " * unique_ptr manages the Impl lifetime (destructor in .cpp)\n";
    std::cout << " * Rule of Five: must declare dtor/move in header, define in .cpp\n";
    return 0;
}
