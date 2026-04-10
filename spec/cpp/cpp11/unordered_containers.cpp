// std::unordered_map / unordered_set / unordered_multimap — C++11
// What: hash-based associative containers with average O(1) lookup, insert, and erase.
// Why:  std::map is a balanced tree (O(log n)) — for many workloads a hash table is faster.
// Pre:  hash containers existed only as compiler extensions (hash_map, hash_set).

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

int main() {
    // unordered_map — key/value lookup
    std::unordered_map<std::string, int> ages;
    ages["Alice"] = 30;
    ages["Bob"]   = 25;
    ages["Carol"] = 27;

    auto it = ages.find("Bob");
    if (it != ages.end()) {
        std::cout << "Bob is " << it->second << "\n";
    }

    // Iteration order is unspecified — it follows the hash distribution
    std::cout << "all entries:\n";
    for (const auto& kv : ages) {
        std::cout << "  " << kv.first << " -> " << kv.second << "\n";
    }

    // unordered_set — membership test
    std::unordered_set<int> primes = {2, 3, 5, 7, 11};
    std::cout << "is 7 prime? "  << (primes.count(7)  ? "yes" : "no") << "\n";
    std::cout << "is 10 prime? " << (primes.count(10) ? "yes" : "no") << "\n";

    // Bucket inspection — useful when measuring hash quality
    std::cout << "ages bucket count: " << ages.bucket_count() << "\n";
    std::cout << "load factor:        " << ages.load_factor() << "\n";

    return 0;
}
