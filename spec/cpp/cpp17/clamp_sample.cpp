// std::clamp / std::sample — C++17
// What: clamp restricts a value to [lo, hi]; sample picks N random elements from a range.
// Why:  both used to be hand-written in every project; now they are standard.
// Pre:  std::min(std::max(v, lo), hi) and ad-hoc reservoir sampling code.

#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <iterator>

int main() {
    // clamp
    int volume = 150;
    int safe   = std::clamp(volume, 0, 100);
    std::cout << "clamp(150, 0, 100) = " << safe << "\n";
    std::cout << "clamp(-5,  0, 100) = " << std::clamp(-5, 0, 100) << "\n";
    std::cout << "clamp(50,  0, 100) = " << std::clamp(50, 0, 100) << "\n";

    // sample — without replacement, into an output iterator
    std::vector<int> population{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<int> chosen;
    std::sample(population.begin(), population.end(),
                std::back_inserter(chosen),
                3,
                std::mt19937{std::random_device{}()});

    std::cout << "sampled 3 of 10: ";
    for (int n : chosen) std::cout << n << " ";
    std::cout << "\n";

    return 0;
}
