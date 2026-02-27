#include <iostream>
#include <vector>
#include <map>

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    for (int i : vec) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::map<std::string, int> map = {{"a", 1}, {"b", 2}, {"c", 3}};
    for (const auto& pair : map) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }

    return 0;
}
