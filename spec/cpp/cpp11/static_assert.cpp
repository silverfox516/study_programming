#include <iostream>

template <typename T>
struct MyType {
    static_assert(sizeof(T) > 4, "T must be larger than 4 bytes");
};

int main() {
    // MyType<int> a; // This will cause a compile-time error
    MyType<long long> b;

    std::cout << "MyType<long long> is created successfully." << std::endl;

    return 0;
}
