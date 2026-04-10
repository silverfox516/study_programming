// <random> — C++11
// What: pluggable random engines + statistical distributions, separated cleanly.
// Why:  rand() is low-quality, has tiny range, and ties you to a global seed.
// Pre:  rand() / srand() — single global state, poor distribution, no engine choice.

#include <iostream>
#include <random>

int main() {
    // Seed once from a non-deterministic source
    std::random_device rd;

    // Mersenne Twister — fast, long period, good for non-cryptographic use
    std::mt19937 engine(rd());

    // Uniform integers in [1, 6] — like a die roll
    std::uniform_int_distribution<int> die(1, 6);
    std::cout << "die rolls: ";
    for (int i = 0; i < 10; ++i) std::cout << die(engine) << " ";
    std::cout << "\n";

    // Uniform real numbers in [0.0, 1.0)
    std::uniform_real_distribution<double> uniform(0.0, 1.0);
    std::cout << "uniform: " << uniform(engine) << " " << uniform(engine) << "\n";

    // Normal distribution — mean 100, stddev 15 (IQ-style)
    std::normal_distribution<double> bell(100.0, 15.0);
    std::cout << "normal samples: ";
    for (int i = 0; i < 5; ++i) std::cout << bell(engine) << " ";
    std::cout << "\n";

    // Bernoulli — true with given probability
    std::bernoulli_distribution coin(0.5);
    int heads = 0;
    for (int i = 0; i < 1000; ++i) if (coin(engine)) ++heads;
    std::cout << "1000 coin flips: " << heads << " heads\n";

    return 0;
}
