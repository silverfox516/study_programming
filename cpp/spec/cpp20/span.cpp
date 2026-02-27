#include <iostream>
#include <span>
#include <vector>

void print_span(std::span<int> s) {
    for (int x : s) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5};
    print_span(arr);

    std::vector<int> vec = {6, 7, 8, 9, 10};
    print_span(vec);

    return 0;
}
