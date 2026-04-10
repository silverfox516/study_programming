// Pipes and Filters — Architectural Pattern
//
// Intent: decompose a complex processing task into a sequence of independent
//         stages (filters) connected by channels (pipes). Each filter reads
//         from its input pipe, transforms the data, and writes to its output pipe.
//
// When to use:
//   - Data flows through a series of transformations (ETL, compilers, media pipelines)
//   - Stages should be reusable, testable, and reorderable independently
//   - You may want to run stages in parallel or on different machines
//
// When NOT to use:
//   - Stages are inherently interactive / stateful across items
//   - The pipeline is so simple that the indirection adds noise

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <cctype>

// ============================================================================
// Pipeline builder (type-erased, using std::function)
// ============================================================================
template <typename T>
class Pipeline {
public:
    using Filter = std::function<T(const T&)>;

    Pipeline& add_filter(Filter f) {
        filters_.push_back(std::move(f));
        return *this;
    }

    T process(T input) const {
        for (const auto& f : filters_) {
            input = f(input);
        }
        return input;
    }

    // Process a batch
    std::vector<T> process_all(std::vector<T> items) const {
        for (auto& item : items) item = process(std::move(item));
        return items;
    }

private:
    std::vector<Filter> filters_;
};

// ============================================================================
// Example: text processing pipeline
// ============================================================================
std::string to_lower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return out;
}

std::string trim(const std::string& s) {
    auto begin = s.find_first_not_of(" \t\n");
    auto end   = s.find_last_not_of(" \t\n");
    if (begin == std::string::npos) return "";
    return s.substr(begin, end - begin + 1);
}

std::string remove_punctuation(const std::string& s) {
    std::string out;
    std::copy_if(s.begin(), s.end(), std::back_inserter(out),
                 [](unsigned char c) { return !std::ispunct(c); });
    return out;
}

int main() {
    std::cout << "=== Pipes and Filters Pattern ===\n";

    Pipeline<std::string> text_pipeline;
    text_pipeline.add_filter(trim)
                 .add_filter(to_lower)
                 .add_filter(remove_punctuation);

    std::vector<std::string> inputs = {
        "  Hello, World!  ",
        "  PIPES & FILTERS!!!  ",
        "Don't Panic.",
    };

    std::cout << "processed:\n";
    for (auto result : text_pipeline.process_all(inputs)) {
        std::cout << "  [" << result << "]\n";
    }

    // Number pipeline
    Pipeline<int> math_pipeline;
    math_pipeline.add_filter([](const int& n) { return n * 2; })
                 .add_filter([](const int& n) { return n + 10; })
                 .add_filter([](const int& n) { return n * n; });

    std::cout << "math pipeline (x*2 + 10)^2:\n";
    for (int x : {1, 2, 3, 5}) {
        std::cout << "  " << x << " -> " << math_pipeline.process(x) << "\n";
    }

    std::cout << "\nKey points:\n";
    std::cout << " * Each filter is a pure function — easy to test and reuse\n";
    std::cout << " * Filters compose by chaining — add/remove/reorder freely\n";
    std::cout << " * Compare with Strategy (dp/cpp/strategy.cpp): Strategy swaps one\n";
    std::cout << "   algorithm; Pipes chains multiple stages\n";
    return 0;
}
