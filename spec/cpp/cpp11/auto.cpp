#include <iostream>
#include <vector>

int main() {
    auto i = 5;
    auto d = 3.14;
    auto s = "Hello, World!";
    
    std::vector<int> vec = {1, 2, 3, 4, 5};
    auto it = vec.begin();
    
    std::cout << "i: " << i << std::endl;
    std::cout << "d: " << d << std::endl;
    std::cout << "s: " << s << std::endl;
    std::cout << "*it: " << *it << std::endl;
    
    return 0;
}
