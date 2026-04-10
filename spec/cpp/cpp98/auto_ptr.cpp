// std::auto_ptr — C++98 (deprecated C++11, removed C++17)
// What: the original single-owner smart pointer. Transfers ownership on copy.
// Why:  needed RAII for heap allocations before unique_ptr existed.
// Pre:  raw new/delete and manual cleanup.
// Why it was removed: copy-as-transfer breaks STL containers and looks identical to a real
//                     copy at the call site. unique_ptr fixes this with explicit move semantics.

#include <iostream>
#include <memory>

class Resource {
public:
    Resource()  { std::cout << "Resource acquired\n"; }
    ~Resource() { std::cout << "Resource released\n"; }
    void use() const { std::cout << "Resource in use\n"; }
};

void take(std::auto_ptr<Resource> p) {
    p->use();
    // p destroyed here — the original auto_ptr at the call site is now NULL
}

int main() {
    std::auto_ptr<Resource> a(new Resource);
    a->use();

    // The dangerous part: copy looks innocent but transfers ownership
    std::auto_ptr<Resource> b = a;   // a is now NULL, b owns the resource
    if (a.get() == 0) {
        std::cout << "After copy, a is NULL (ownership moved to b)\n";
    }
    b->use();

    // Same issue when passing by value — caller's auto_ptr becomes NULL
    take(b);
    if (b.get() == 0) {
        std::cout << "After call, b is NULL (ownership moved into take())\n";
    }

    // Why it cannot live in std::vector: the container assumes copy-as-copy,
    // but auto_ptr does copy-as-move. Many algorithms become silently broken.
    // std::vector<std::auto_ptr<Resource> > vec;  // legal but a footgun

    return 0;
}
