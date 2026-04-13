// C++17: map::try_emplace and insert_or_assign — safer map insertion

#include <iostream>
#include <map>
#include <string>

struct Expensive {
    std::string data;
    Expensive(std::string d) : data(std::move(d)) {
        std::cout << "  Constructed: " << data << "\n";
    }
    Expensive(Expensive&& o) noexcept : data(std::move(o.data)) {
        std::cout << "  Moved: " << data << "\n";
    }
};

int main() {
    std::map<int, Expensive> m;

    // try_emplace: does NOT construct the value if key already exists
    std::cout << "--- try_emplace(1, \"first\") ---\n";
    auto [it1, ok1] = m.try_emplace(1, "first");
    std::cout << "  inserted: " << ok1 << "\n";

    std::cout << "--- try_emplace(1, \"second\") ---\n";
    auto [it2, ok2] = m.try_emplace(1, "second");  // "second" NOT constructed
    std::cout << "  inserted: " << ok2 << "\n";
    std::cout << "  value: " << it2->second.data << "\n";

    // vs emplace: would construct then discard if key exists
    // m.emplace(1, "wasteful");  // constructs "wasteful" even if key 1 exists

    // insert_or_assign: insert or overwrite
    std::cout << "\n--- insert_or_assign(2, \"new\") ---\n";
    auto [it3, ok3] = m.insert_or_assign(2, Expensive("new"));
    std::cout << "  inserted: " << ok3 << "\n";

    std::cout << "--- insert_or_assign(2, \"updated\") ---\n";
    auto [it4, ok4] = m.insert_or_assign(2, Expensive("updated"));
    std::cout << "  inserted(=replaced): " << ok4 << "\n";
    std::cout << "  value: " << it4->second.data << "\n";

    // Summary
    std::cout << "\nFinal map:\n";
    for (const auto& [k, v] : m) {
        std::cout << "  " << k << " -> " << v.data << "\n";
    }

    return 0;
}
