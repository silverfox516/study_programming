// std::out_ptr / std::inout_ptr — C++23
// What: adapters that let smart pointers participate in C-style "T**" out-parameter APIs.
// Why:  C interfaces like `int create(Foo** out)` were awkward to call from
//       smart-pointer-owning C++ code without leaking on errors.
// Pre:  raw temporary pointer + manual reset() with try/catch for safety.

#include <iostream>
#include <memory>
#include <cstdlib>

// A C-style API that allocates and returns via out-parameter
extern "C" int legacy_create(int** out_ptr) {
    *out_ptr = static_cast<int*>(std::malloc(sizeof(int)));
    if (*out_ptr) {
        **out_ptr = 42;
        return 0;   // success
    }
    return -1;
}

extern "C" void legacy_destroy(int* p) {
    std::free(p);
}

// A custom deleter that calls legacy_destroy
struct LegacyDeleter {
    void operator()(int* p) const noexcept { legacy_destroy(p); }
};

int main() {
    std::unique_ptr<int, LegacyDeleter> owned;

    // out_ptr lets the C function fill `owned` directly
    int rc = legacy_create(std::out_ptr(owned));
    if (rc == 0 && owned) {
        std::cout << "C API gave us: " << *owned << "\n";
    }

    // owned will release via LegacyDeleter when it goes out of scope
    return 0;
}
