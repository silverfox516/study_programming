// std::flat_map / std::flat_set — C++23
// What: associative containers backed by a sorted contiguous array (vector by default)
//       instead of a node-based tree.
// Why:  cache-friendly iteration and lower memory overhead than std::map/std::set,
//       at the cost of slower per-element insertion (shifts).
// Pre:  hand-rolled "sorted vector + binary_search" patterns in performance-sensitive code.

#include <iostream>
#include <flat_map>
#include <flat_set>
#include <string>

int main() {
    std::flat_map<std::string, int> ages;
    ages.try_emplace("Alice", 30);
    ages.try_emplace("Bob",   25);
    ages.try_emplace("Carol", 27);

    // Iteration is in sorted-by-key order (because storage is sorted)
    for (const auto& [name, age] : ages) {
        std::cout << name << " -> " << age << "\n";
    }

    auto it = ages.find("Bob");
    if (it != ages.end()) {
        std::cout << "found Bob: " << it->second << "\n";
    }

    std::flat_set<int> primes{2, 3, 5, 7, 11, 13};
    std::cout << "primes contains 7: " << primes.contains(7) << "\n";
    std::cout << "size:               " << primes.size()     << "\n";

    // The big benefit: storage is contiguous — better for the cache
    // and friendlier to allocators.

    return 0;
}
