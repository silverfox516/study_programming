// Iterator categories — C++98
// What: the five iterator tags that classify what an iterator can do.
// Why:  algorithms can pick a fast or slow implementation by tag-dispatching on capability.
// Pre:  there was no other way; iterator categories ARE the C++98 mechanism.
// Modern: C++20 ranges replace tag dispatch with concepts (input_iterator, random_access_iterator).

#include <iostream>
#include <iterator>
#include <vector>
#include <list>

// Tag-dispatched advance: O(n) for everything except random-access (O(1))
template <typename It>
void my_advance_impl(It& it, int n, std::input_iterator_tag) {
    std::cout << "  [linear advance]\n";
    while (n-- > 0) ++it;
}

template <typename It>
void my_advance_impl(It& it, int n, std::random_access_iterator_tag) {
    std::cout << "  [constant-time advance]\n";
    it += n;
}

template <typename It>
void my_advance(It& it, int n) {
    typedef typename std::iterator_traits<It>::iterator_category Tag;
    my_advance_impl(it, n, Tag());
}

int main() {
    // Random-access iterator — vector
    std::vector<int> v;
    for (int i = 0; i < 10; ++i) v.push_back(i);
    std::vector<int>::iterator vit = v.begin();
    std::cout << "vector iterator:\n";
    my_advance(vit, 5);
    std::cout << "  *vit = " << *vit << "\n";

    // Bidirectional iterator — list (linear advance)
    std::list<int> l;
    for (int i = 0; i < 10; ++i) l.push_back(i * 10);
    std::list<int>::iterator lit = l.begin();
    std::cout << "list iterator:\n";
    my_advance(lit, 5);
    std::cout << "  *lit = " << *lit << "\n";

    // istream_iterator is an input iterator — single-pass, no going back
    std::cout << "iterator_traits give us value_type, difference_type, "
              << "pointer, reference, and iterator_category for any iterator.\n";

    return 0;
}
