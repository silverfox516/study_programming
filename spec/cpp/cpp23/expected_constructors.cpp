// std::expected construction patterns — C++23
// What: how to build expected values for both the success and the error path,
//       including in-place construction and the std::unexpected helper.
// Why:  knowing the construction options is half of using expected fluently.
// See also: spec/cpp/cpp23/expected.cpp for basic access.

#include <iostream>
#include <expected>
#include <string>

struct Error {
    int         code;
    std::string message;
};

// Implicit success construction
std::expected<int, Error> ok_value() {
    return 42;
}

// Explicit error via std::unexpected
std::expected<int, Error> err_simple() {
    return std::unexpected(Error{42, "boom"});
}

// In-place construction of the value (avoid an extra copy/move)
std::expected<std::string, Error> ok_inplace() {
    return std::expected<std::string, Error>(std::in_place, 5, 'a');  // "aaaaa"
}

// In-place construction of the error
std::expected<int, Error> err_inplace() {
    return std::expected<int, Error>(std::unexpect, 7, "in-place error");
}

int main() {
    auto a = ok_value();
    auto b = err_simple();
    auto c = ok_inplace();
    auto d = err_inplace();

    auto show = [](const auto& r) {
        if (r) std::cout << "ok: " << *r << "\n";
        else   std::cout << "err: " << r.error().code << " - " << r.error().message << "\n";
    };

    show(a);
    show(b);
    show(c);
    show(d);

    return 0;
}
