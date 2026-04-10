// std::integer_sequence / std::index_sequence — C++14
// What: a compile-time sequence of integers encoded as a type's template parameter pack.
// Why:  the canonical way to "unpack" a tuple by indexing into it from a parameter pack
//       expansion. Foundation for std::apply (C++17) and many template tricks.
// Pre:  hand-rolled IndexSequence helpers, copy-pasted into every project that needed it.

#include <iostream>
#include <tuple>
#include <utility>

// Apply a function to a tuple by expanding indices into the get<I> calls
template <typename Tuple, typename F, std::size_t... Is>
void for_each_impl(const Tuple& t, F f, std::index_sequence<Is...>) {
    // Pack expansion produces: f(get<0>(t)), f(get<1>(t)), ...
    int dummy[] = { (f(std::get<Is>(t)), 0)... };
    (void)dummy;
}

template <typename Tuple, typename F>
void for_each(const Tuple& t, F f) {
    for_each_impl(t, f, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
}

struct Print {
    template <typename T>
    void operator()(const T& v) const { std::cout << v << " "; }
};

int main() {
    auto t = std::make_tuple(1, "hello", 3.14, 'X');
    for_each(t, Print{});
    std::cout << "\n";

    // Inspect the sequence type itself
    using Seq = std::make_index_sequence<5>;
    std::cout << "Seq has " << Seq::size() << " elements\n";

    return 0;
}
