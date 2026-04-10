// if (init; condition) and switch (init; expr) — C++17
// What: declare and initialize variables in the if/switch header itself, scoped
//       only to the if/else (or switch) body.
// Why:  keeps the variable's scope as small as possible; matches the for-loop pattern.
// Pre:  the variable had to be declared in the enclosing scope and stayed visible after.

#include <iostream>
#include <map>
#include <string>
#include <mutex>

int main() {
    std::map<std::string, int> ages = {{"Alice", 30}, {"Bob", 25}};

    // Old way:
    // auto it = ages.find("Alice");
    // if (it != ages.end()) { ... }
    //   // 'it' still visible here, polluting the scope

    // C++17 way: 'it' is scoped to the if/else block only
    if (auto it = ages.find("Alice"); it != ages.end()) {
        std::cout << it->first << " is " << it->second << "\n";
    } else {
        std::cout << "not found\n";
    }
    // 'it' is gone here

    // switch with init: declare a guard alongside the switched value
    static std::mutex m;
    switch (std::lock_guard<std::mutex> lock(m); ages.size()) {
    case 1: std::cout << "one entry\n";   break;
    case 2: std::cout << "two entries\n"; break;
    default: std::cout << "other count\n"; break;
    }

    return 0;
}
