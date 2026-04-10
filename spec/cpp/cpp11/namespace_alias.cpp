// Namespace alias — C++11 (the syntax existed in C++98 too, but it pairs well with
//                          C++11 long namespace paths in <chrono>, <filesystem>, etc.)
// What: give a long namespace path a short local name.
// Why:  reduces typing without polluting the global scope like `using namespace` would.
// Pre:  always typing the full namespace path or doing a `using namespace`.

#include <iostream>
#include <chrono>

namespace very { namespace deep { namespace ns {
    int compute() { return 42; }
}}}

int main() {
    // Local alias for chrono — common in real code
    namespace chr = std::chrono;

    auto start = chr::steady_clock::now();
    int total = 0;
    for (int i = 0; i < 1000; ++i) total += i;
    auto elapsed = chr::duration_cast<chr::microseconds>(chr::steady_clock::now() - start);
    std::cout << "sum=" << total << " in " << elapsed.count() << " us\n";

    // Alias for our own deeply-nested namespace
    namespace vd = very::deep::ns;
    std::cout << "vd::compute() = " << vd::compute() << "\n";

    return 0;
}
