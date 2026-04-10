// std::exchange — C++14
// What: assigns a new value to an object and returns the old value, in one call.
// Why:  the canonical way to write a move constructor's "leave the source empty"
//       step, and a clean way to swap-and-return idioms.
// Pre:  three-line temp/swap pattern repeated everywhere.

#include <iostream>
#include <utility>

class Buffer {
public:
    explicit Buffer(std::size_t n) : data_(new int[n]), size_(n) {}
    ~Buffer() { delete[] data_; }

    Buffer(Buffer&& other) noexcept
        : data_(std::exchange(other.data_, nullptr)),
          size_(std::exchange(other.size_, 0)) {}

    std::size_t size() const { return size_; }
    bool        empty() const { return data_ == nullptr; }

private:
    int*        data_;
    std::size_t size_;
};

int main() {
    int counter = 5;
    int prev = std::exchange(counter, 10);
    std::cout << "prev=" << prev << " new=" << counter << "\n";

    Buffer a(100);
    Buffer b = std::move(a);
    std::cout << "a.empty()=" << a.empty() << " b.size()=" << b.size() << "\n";

    return 0;
}
