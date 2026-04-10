// Uniform initialization (brace-init) — C++11
// What: {} can initialize anything — primitives, aggregates, classes, containers — uniformly.
// Why:  one syntax for everything, and crucially it disallows narrowing conversions
//       that would silently lose information.
// Pre:  () for class init, = for primitives, {} for arrays — three different rules.

#include <iostream>
#include <vector>
#include <map>
#include <string>

struct Point {
    int x;
    int y;
};

class Widget {
public:
    Widget(int n, std::string label) : n_(n), label_(std::move(label)) {
        std::cout << "Widget(" << n_ << ", '" << label_ << "')\n";
    }
private:
    int         n_;
    std::string label_;
};

int main() {
    // Primitives
    int    i{42};
    double d{3.14};
    std::cout << i << " " << d << "\n";

    // Aggregate (no constructor)
    Point p{1, 2};
    std::cout << "Point: " << p.x << "," << p.y << "\n";

    // Class with constructor
    Widget w{7, "alpha"};

    // Containers — uses initializer_list constructor
    std::vector<int>            v{10, 20, 30};
    std::map<std::string, int>  m{{"one", 1}, {"two", 2}};
    std::cout << "v.size = " << v.size() << ", m[\"one\"] = " << m["one"] << "\n";

    // Narrowing is rejected: this would refuse to compile
    // int bad{3.14};  // error: narrowing conversion from double to int

    // Whereas the old syntax silently allowed it. The warning emitted here is
    // exactly the point of this example — () lets it through, {} would refuse.
    double pi = 3.14;
    int legacy(pi);  // truncates to 3
    std::cout << "legacy = " << legacy << "\n";

    return 0;
}
