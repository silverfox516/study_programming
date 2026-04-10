// Extended friend declarations — C++11
// What: a class can declare a template parameter type as a friend, without writing
//       `friend class T` (which used to require T to be a class type at parse time).
// Why:  enables generic friend grants in template wrappers and CRTP-style designs.
// Pre:  you had to know the friend's exact name; you could not say "whatever T is".

#include <iostream>

template <typename T>
class Wrapper {
public:
    explicit Wrapper(int v) : value_(v) {}

    // C++11 lets us befriend the template parameter directly.
    // If T turns out to be a non-class type, the friend declaration is harmless.
    friend T;

private:
    int value_;
};

class Inspector {
public:
    void peek(const Wrapper<Inspector>& w) const {
        // Allowed because Inspector was declared a friend of Wrapper<Inspector>
        std::cout << "peeked value: " << w.value_ << "\n";
    }
};

int main() {
    Wrapper<Inspector> w(42);
    Inspector{}.peek(w);

    // Wrapper<int> compiles fine — the friend declaration just has no effect.
    Wrapper<int> wi(7);
    (void)wi;
    std::cout << "Wrapper<int> instantiated ok\n";

    return 0;
}
