#include <iostream>
#include <vector>
#include <string>
#include <utility>

void print_string(const std::string& s) {
    std::cout << "lvalue reference: " << s << std::endl;
}

void print_string(std::string&& s) {
    std::cout << "rvalue reference: " << s << std::endl;
}

int main() {
    std::string s1 = "hello";
    print_string(s1);
    print_string("world");
    print_string(std::move(s1));
    
    std::cout << "After move, s1 is: \"" << s1 << "\"" << std::endl;

    return 0;
}
