#include <iostream>
#include <optional>

struct User {
    std::optional<std::string> name;
};

std::optional<User> get_user(int id) {
    if (id == 1) {
        return User{std::make_optional("Alice")};
    }
    return std::nullopt;
}

int main() {
    auto name = get_user(1)
        .and_then([](const User& u) { return u.name; });

    if (name) {
        std::cout << "User name: " << *name << std::endl;
    } else {
        std::cout << "User not found or has no name." << std::endl;
    }
    
    auto name2 = get_user(2)
        .and_then([](const User& u) { return u.name; });

    if (!name2) {
        std::cout << "User not found or has no name." << std::endl;
    }

    return 0;
}
