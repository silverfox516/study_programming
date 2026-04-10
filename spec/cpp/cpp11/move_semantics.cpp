// Move semantics — C++11
// What: std::move converts an lvalue into an rvalue, enabling cheap transfer of resources.
// Why:  copying a large object is wasteful when the source is about to be destroyed anyway.
// Pre:  every transfer was a deep copy, or a hand-rolled "swap" trick.

#include <iostream>
#include <vector>
#include <string>
#include <utility>

class Buffer {
public:
    explicit Buffer(std::size_t n) : size_(n), data_(new int[n]) {
        std::cout << "ctor(" << size_ << ")\n";
    }
    ~Buffer() {
        std::cout << "dtor(" << size_ << ")\n";
        delete[] data_;
    }

    // Copy: deep copy
    Buffer(const Buffer& other) : size_(other.size_), data_(new int[other.size_]) {
        std::cout << "copy(" << size_ << ")\n";
    }

    // Move: steal pointer, leave source in a valid empty state
    Buffer(Buffer&& other) noexcept : size_(other.size_), data_(other.data_) {
        std::cout << "move(" << size_ << ")\n";
        other.size_ = 0;
        other.data_ = nullptr;
    }

private:
    std::size_t size_;
    int*        data_;
};

Buffer make_buffer() {
    return Buffer(100);  // returned by value — typically elided or moved
}

int main() {
    Buffer a(10);
    Buffer b = a;             // copy
    Buffer c = std::move(a);  // move — a is now empty but still destructible

    Buffer d = make_buffer(); // construction from rvalue — move (or elision)

    // std::vector uses move under the hood — pushing an rvalue avoids the copy
    std::vector<std::string> v;
    std::string s = "hello";
    v.push_back(std::move(s));   // s is now empty
    std::cout << "after move, s = '" << s << "'\n";
    std::cout << "v[0] = '" << v[0] << "'\n";

    return 0;
}
