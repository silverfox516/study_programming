#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    fs::path p = fs::current_path();
    std::cout << "Current path: " << p << std::endl;

    for (const auto& entry : fs::directory_iterator(p)) {
        std::cout << entry.path() << std::endl;
    }

    return 0;
}
