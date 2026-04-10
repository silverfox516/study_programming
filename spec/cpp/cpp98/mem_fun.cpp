// std::mem_fun / std::mem_fun_ref — C++98 (removed in C++17)
// What: adapters that turn a member function into something an STL algorithm can call.
// Why:  std::for_each cannot call obj.method() directly — it needs a unary functor.
// Pre:  the only alternative was a hand-written wrapping functor for every member function.
// Modern: std::mem_fn (C++11) replaces both, and lambdas/std::invoke are usually clearer.

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

class Widget {
public:
    explicit Widget(int id) : id_(id) {}
    void show() const { std::cout << "Widget " << id_ << "\n"; }
    int  id()   const { return id_; }
private:
    int id_;
};

int main() {
    // Container of pointers — mem_fun expects a pointer-to-object as argument
    std::vector<Widget*> ptrs;
    ptrs.push_back(new Widget(1));
    ptrs.push_back(new Widget(2));
    ptrs.push_back(new Widget(3));

    std::cout << "Via mem_fun (pointer container):\n";
    std::for_each(ptrs.begin(), ptrs.end(), std::mem_fun(&Widget::show));

    // Container of values — use mem_fun_ref instead
    std::vector<Widget> objs;
    objs.push_back(Widget(10));
    objs.push_back(Widget(20));

    std::cout << "Via mem_fun_ref (value container):\n";
    std::for_each(objs.begin(), objs.end(), std::mem_fun_ref(&Widget::show));

    // Cleanup
    for (std::vector<Widget*>::iterator it = ptrs.begin(); it != ptrs.end(); ++it) {
        delete *it;
    }

    return 0;
}
