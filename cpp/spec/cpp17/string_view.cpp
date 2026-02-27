#include <iostream>
#include <string_view>

void print_sv(std::string_view sv) {
    std::cout << sv << std::endl;
}

int main() {
    std::string s = "Hello, World!";
    std::string_view sv1 = s;
    
    const char* c_str = "This is a C-style string.";
    std::string_view sv2(c_str);

    print_sv(sv1);
    print_sv(sv2);
    print_sv("This is a literal.");

    return 0;
}
