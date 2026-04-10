// <memory_resource> (PMR) — C++17
// What: polymorphic memory resources — runtime-polymorphic allocators that don't change
//       a container's type. Lets you swap allocation strategy without templating everything.
// Why:  C++03/11 allocators were a template parameter, so two vectors with different
//       allocators were different types. PMR lifts the allocator out of the type system.
// Pre:  custom allocators meant viral template parameters everywhere.

#include <iostream>
#include <memory_resource>
#include <vector>
#include <array>

int main() {
    // Stack-backed buffer — no heap allocation for small workloads
    std::array<std::byte, 1024> buffer;
    std::pmr::monotonic_buffer_resource pool(buffer.data(), buffer.size());

    // Same vector type whether the allocator is heap or stack
    std::pmr::vector<int> v(&pool);
    for (int i = 0; i < 10; ++i) v.push_back(i * i);

    std::cout << "vector size: " << v.size() << "\n";
    for (int x : v) std::cout << x << " ";
    std::cout << "\n";

    // Default resource = the global heap
    std::pmr::vector<int> heap_v(std::pmr::get_default_resource());
    heap_v.push_back(42);
    std::cout << "heap vector first: " << heap_v.front() << "\n";

    // Both have the same type — std::pmr::vector<int>
    static_assert(std::is_same<decltype(v), decltype(heap_v)>::value, "");

    return 0;
}
