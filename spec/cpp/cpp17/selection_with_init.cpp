// C++17: switch with initializer — complement to if-with-init

#include <iostream>
#include <string>
#include <map>

enum class Status { OK, Warning, Error, Unknown };

Status get_status(int code) {
    if (code < 200) return Status::Unknown;
    if (code < 300) return Status::OK;
    if (code < 400) return Status::Warning;
    return Status::Error;
}

int main() {
    // switch with initializer
    switch (auto s = get_status(302); s) {
        case Status::OK:
            std::cout << "OK\n";
            break;
        case Status::Warning:
            std::cout << "Warning (redirect)\n";
            break;
        case Status::Error:
            std::cout << "Error\n";
            break;
        case Status::Unknown:
            std::cout << "Unknown\n";
            break;
    }

    // if with initializer (recap) — scope-limited variable
    std::map<std::string, int> scores{{"alice", 95}, {"bob", 82}};

    if (auto it = scores.find("alice"); it != scores.end()) {
        std::cout << "Found: " << it->first << " = " << it->second << "\n";
    }
    // 'it' is not accessible here

    // switch with initializer — lock guard pattern
    switch (int val = 42 % 3; val) {
        case 0: std::cout << "42 is divisible by 3\n"; break;
        case 1: std::cout << "42 mod 3 = 1\n"; break;
        case 2: std::cout << "42 mod 3 = 2\n"; break;
    }

    return 0;
}
