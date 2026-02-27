#include <iostream>
#include <typeinfo>

int x = 1;
int& get_ref() { return x; }

decltype(auto) get_ref_decltype_auto() {
    return get_ref();
}

int main() {
    decltype(auto) y = get_ref_decltype_auto();
    std::cout << "Type of y: " << typeid(y).name() << std::endl;
    y = 100;
    std::cout << "x: " << x << std::endl;
    return 0;
}
