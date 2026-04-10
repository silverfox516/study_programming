// = default and = delete — C++11
// What: explicitly request the compiler-generated version of a special member function,
//       or explicitly forbid a function (any function, not only specials).
// Why:  expresses intent at the declaration site; deleting a copy ctor is the standard way
//       to make a class non-copyable.
// Pre:  non-copyable classes inherited from boost::noncopyable or made the copy ctor private+unimplemented.

#include <iostream>

class Tracked {
public:
    Tracked()                            = default;
    Tracked(const Tracked&)              = default;
    Tracked& operator=(const Tracked&)   = default;
    ~Tracked()                           = default;
};

class Unique {
public:
    Unique() {}
    // Forbid copying — lets the compiler diagnose any attempt
    Unique(const Unique&)            = delete;
    Unique& operator=(const Unique&) = delete;
};

// Delete also works on free functions to forbid specific overloads
void only_int(int n) { std::cout << "only_int got " << n << "\n"; }
void only_int(double) = delete;   // forbid implicit double->int conversion

int main() {
    Tracked a;
    Tracked b = a;        // ok — defaulted copy
    (void)b;

    Unique u;
    // Unique u2 = u;     // would fail to compile — copy is deleted
    (void)u;

    only_int(42);
    // only_int(3.14);    // would fail to compile — overload deleted

    std::cout << "compiled ok\n";
    return 0;
}
