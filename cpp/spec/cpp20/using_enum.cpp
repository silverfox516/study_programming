#include <iostream>

enum class Color {
    Red,
    Green,
    Blue
};

void print_color(Color c) {
    using enum Color;
    switch (c) {
        case Red:
            std::cout << "It's Red" << std::endl;
            break;
        case Green:
            std::cout << "It's Green" << std::endl;
            break;
        case Blue:
            std::cout << "It's Blue" << std::endl;
            break;
    }
}

int main() {
    Color c = Color::Red;
    print_color(c);
    return 0;
}
