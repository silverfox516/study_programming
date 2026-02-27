#include <iostream>

enum class Color { Red, Green, Blue };
enum class Alert { Green, Yellow, Red };

void print_color(Color color) {
    if (color == Color::Red) {
        std::cout << "Color is Red" << std::endl;
    }
}

int main() {
    Color c = Color::Red;
    // int i = c; // Error: cannot convert Color to int
    // if (c == Alert::Red) // Error: cannot compare Color and Alert
    
    print_color(c);

    return 0;
}
