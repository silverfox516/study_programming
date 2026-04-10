// Function objects (functors) — C++98
// What: a struct/class with operator() that behaves like a callable.
// Why:  the only way to carry per-call state into an STL algorithm before lambdas (C++11).
// Pre:  free functions worked but could not capture state without globals.

#include <iostream>
#include <vector>
#include <algorithm>

// Stateless functor — equivalent to a free function
struct Print {
    void operator()(int n) const {
        std::cout << n << " ";
    }
};

// Stateful functor — captures a threshold via the constructor
struct GreaterThan {
    int threshold;
    explicit GreaterThan(int t) : threshold(t) {}
    bool operator()(int n) const {
        return n > threshold;
    }
};

// Stateful functor that accumulates — shows why state matters
struct Counter {
    int count;
    Counter() : count(0) {}
    void operator()(int) { ++count; }
};

int main() {
    std::vector<int> v;
    v.push_back(3); v.push_back(1); v.push_back(4);
    v.push_back(1); v.push_back(5); v.push_back(9);

    std::cout << "All: ";
    std::for_each(v.begin(), v.end(), Print());
    std::cout << "\n";

    std::vector<int>::iterator it =
        std::find_if(v.begin(), v.end(), GreaterThan(4));
    if (it != v.end()) {
        std::cout << "First element > 4: " << *it << "\n";
    }

    // for_each returns a copy of the functor, so we can read accumulated state
    Counter c = std::for_each(v.begin(), v.end(), Counter());
    std::cout << "Counted " << c.count << " elements\n";

    return 0;
}
