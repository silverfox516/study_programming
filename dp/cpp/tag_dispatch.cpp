// Tag Dispatch — C++ Idiom
//
// Intent: select an overload at compile time by passing a tag type (an empty
//         struct) as an extra argument. The tag encodes a compile-time decision
//         so the right implementation is picked by overload resolution.
//
// When to use:
//   - You need to dispatch on a trait or capability at compile time
//   - The dispatched functions have the same name and arity
//   - Pre-C++20 — concepts now cover many tag-dispatch use cases more cleanly
//
// When NOT to use:
//   - C++20 concepts + constrained overloads are available and clearer
//   - Runtime dispatch is needed — use virtual functions or variant

#include <iostream>
#include <iterator>
#include <vector>
#include <list>
#include <type_traits>

// ============================================================================
// Example 1: advance dispatch on iterator category (std::advance does this)
// ============================================================================
namespace detail {
    template <typename It>
    void advance_impl(It& it, int n, std::random_access_iterator_tag) {
        std::cout << "  [O(1)] random-access advance\n";
        it += n;
    }
    template <typename It>
    void advance_impl(It& it, int n, std::input_iterator_tag) {
        std::cout << "  [O(n)] linear advance\n";
        while (n-- > 0) ++it;
    }
}

template <typename It>
void my_advance(It& it, int n) {
    detail::advance_impl(it, n, typename std::iterator_traits<It>::iterator_category{});
}

// ============================================================================
// Example 2: custom tag dispatch for serialization format
// ============================================================================
struct JsonTag {};
struct XmlTag  {};
struct CsvTag  {};

template <typename T>
void serialize(const T& value, JsonTag) {
    std::cout << "  {\"value\": \"" << value << "\"}\n";
}

template <typename T>
void serialize(const T& value, XmlTag) {
    std::cout << "  <value>" << value << "</value>\n";
}

template <typename T>
void serialize(const T& value, CsvTag) {
    std::cout << "  value," << value << "\n";
}

// ============================================================================
// Example 3: dispatch on type traits
// ============================================================================
struct TrivialTag {};
struct NonTrivialTag {};

template <typename T>
using TriviallyCopyableTag = std::conditional_t<
    std::is_trivially_copyable_v<T>, TrivialTag, NonTrivialTag>;

template <typename T>
void copy_elements(const T* src, T* dst, std::size_t n, TrivialTag) {
    std::cout << "  memcpy (trivially copyable)\n";
    std::memcpy(dst, src, n * sizeof(T));
}

template <typename T>
void copy_elements(const T* src, T* dst, std::size_t n, NonTrivialTag) {
    std::cout << "  element-wise copy (non-trivial)\n";
    for (std::size_t i = 0; i < n; ++i) dst[i] = src[i];
}

template <typename T>
void smart_copy(const T* src, T* dst, std::size_t n) {
    copy_elements(src, dst, n, TriviallyCopyableTag<T>{});
}

int main() {
    std::cout << "=== Tag Dispatch Pattern ===\n";

    // Iterator dispatch
    std::vector<int> v{1, 2, 3, 4, 5};
    auto vit = v.begin();
    my_advance(vit, 3);
    std::cout << "  vector[3] = " << *vit << "\n";

    std::list<int> l{10, 20, 30, 40};
    auto lit = l.begin();
    my_advance(lit, 2);
    std::cout << "  list[2] = " << *lit << "\n";

    // Serialization dispatch
    serialize(42, JsonTag{});
    serialize(42, XmlTag{});
    serialize(42, CsvTag{});

    // Trivial copy dispatch
    int src[] = {1, 2, 3};
    int dst[3];
    smart_copy(src, dst, 3);

    std::string s_src[] = {"a", "b"};
    std::string s_dst[2];
    smart_copy(s_src, s_dst, 2);

    std::cout << "\nKey points:\n";
    std::cout << " * Tags are empty structs — zero runtime cost\n";
    std::cout << " * Overload resolution picks the right function at compile time\n";
    std::cout << " * The standard library uses this for iterator categories\n";
    std::cout << " * C++20 concepts often replace tag dispatch more cleanly\n";
    return 0;
}
