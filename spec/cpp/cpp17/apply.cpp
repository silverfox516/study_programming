#include <iostream>
#include <tuple>
#include <utility>

int add(int a, int b) {
    return a + b;
}

int main() {
    std::tuple<int, int> args = {5, 10};
    
    int result = std::apply(add, args);

    std::cout << "Result: " << result << std::endl;

    return 0;
}
