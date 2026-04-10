// std::make_unique — C++14
// What: factory function that constructs an object on the heap and returns a unique_ptr.
// Why:  C++11 had make_shared but no make_unique — an oversight finally fixed in C++14.
//       Avoids raw `new` at the call site and is exception-safe in argument lists.
// Pre:  std::unique_ptr<T> p(new T(args)); — manual `new` outside of RAII.

#include <iostream>
#include <memory>
#include <string>

class Resource {
public:
    Resource(const std::string& name, int n) : name_(name), n_(n) {
        std::cout << "Resource(" << name_ << "," << n_ << ")\n";
    }
    ~Resource() { std::cout << "~Resource(" << name_ << ")\n"; }
    void show() const { std::cout << name_ << " = " << n_ << "\n"; }
private:
    std::string name_;
    int         n_;
};

void take_two(std::unique_ptr<Resource> a, std::unique_ptr<Resource> b) {
    a->show();
    b->show();
}

int main() {
    auto p = std::make_unique<Resource>("alpha", 1);
    p->show();

    // Array form: make_unique<T[]>(N) creates unique_ptr<T[]>
    auto arr = std::make_unique<int[]>(5);
    for (int i = 0; i < 5; ++i) arr[i] = i * i;
    for (int i = 0; i < 5; ++i) std::cout << arr[i] << " ";
    std::cout << "\n";

    // Why this matters: with raw `new`, an exception between two `new` calls in
    // the same expression could leak. make_unique sequences each construction
    // safely.
    take_two(std::make_unique<Resource>("beta",  2),
             std::make_unique<Resource>("gamma", 3));

    return 0;
}
