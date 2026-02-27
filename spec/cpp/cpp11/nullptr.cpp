#include <iostream>

void foo(int* p) {
    std::cout << "foo(int*)" << std::endl;
}

void foo(char* p) {
    std::cout << "foo(char*)" << std::endl;
}

int main() {
    int* p1 = nullptr;
    char* p2 = nullptr;

    // foo(NULL); // Ambiguous call
    foo(p1);
    foo(p2);
    
    if (p1 == nullptr) {
        std::cout << "p1 is a null pointer" << std::endl;
    }

    return 0;
}
