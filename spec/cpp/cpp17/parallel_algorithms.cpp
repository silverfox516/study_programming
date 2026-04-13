// C++17 Parallel Algorithms (std::execution policies)
// Note: Apple Clang does not support <execution> — guarded with __has_include

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

#if __has_include(<execution>)
#include <execution>
#define HAS_EXECUTION 1
#else
#define HAS_EXECUTION 0
#endif

int main() {
    std::vector<int> v(100);
    std::iota(v.begin(), v.end(), 1); // 1..100

#if HAS_EXECUTION && defined(__cpp_lib_execution)
    // parallel sort
    std::sort(std::execution::par, v.begin(), v.end(), std::greater<>{});
    std::cout << "Parallel sort: first = " << v.front()
              << ", last = " << v.back() << "\n";

    // parallel for_each
    std::atomic<long long> sum{0};
    std::for_each(std::execution::par, v.begin(), v.end(),
                  [&sum](int x) { sum += x; });
    std::cout << "Parallel sum: " << sum.load() << "\n";

    // parallel reduce
    auto total = std::reduce(std::execution::par, v.begin(), v.end(), 0LL);
    std::cout << "Parallel reduce: " << total << "\n";
#else
    // fallback: sequential
    std::sort(v.begin(), v.end(), std::greater<>{});
    std::cout << "[no <execution> support — sequential fallback]\n";
    std::cout << "Sort: first = " << v.front()
              << ", last = " << v.back() << "\n";

    long long sum = 0;
    for (auto x : v) sum += x;
    std::cout << "Sum: " << sum << "\n";
#endif

    return 0;
}
