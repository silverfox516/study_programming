// Pipe & Filter Architecture
// Data flows through a pipeline of independent processing stages.
// Each filter transforms input to output; pipes connect them.

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <sstream>
#include <cctype>

// ============================================================
// Generic pipeline infrastructure
// ============================================================
template <typename T>
class Pipeline {
    std::vector<std::function<T(T)>> filters_;
public:
    Pipeline& add(std::function<T(T)> filter) {
        filters_.push_back(std::move(filter));
        return *this;
    }

    T process(T input) const {
        T result = std::move(input);
        for (const auto& filter : filters_)
            result = filter(std::move(result));
        return result;
    }
};

// ============================================================
// Example 1: Text processing pipeline
// ============================================================
namespace text_pipeline {

std::string trim(std::string s) {
    auto start = s.find_first_not_of(" \t\n");
    auto end = s.find_last_not_of(" \t\n");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

std::string to_lower(std::string s) {
    for (auto& c : s) c = std::tolower(c);
    return s;
}

std::string remove_punctuation(std::string s) {
    s.erase(std::remove_if(s.begin(), s.end(), ::ispunct), s.end());
    return s;
}

std::string normalize_spaces(std::string s) {
    std::string result;
    bool last_space = false;
    for (char c : s) {
        if (c == ' ') {
            if (!last_space) result += c;
            last_space = true;
        } else {
            result += c;
            last_space = false;
        }
    }
    return result;
}

void demo() {
    Pipeline<std::string> pipeline;
    pipeline
        .add(trim)
        .add(to_lower)
        .add(remove_punctuation)
        .add(normalize_spaces);

    std::vector<std::string> inputs = {
        "  Hello,  World!  ",
        "   PIPES   AND   FILTERS!!!  ",
        "  Clean Architecture...  ",
    };

    std::cout << "=== Text Pipeline ===\n";
    for (const auto& input : inputs) {
        auto output = pipeline.process(input);
        std::cout << "  \"" << input << "\" -> \"" << output << "\"\n";
    }
}

} // namespace text_pipeline

// ============================================================
// Example 2: Data pipeline (batch processing)
// ============================================================
namespace data_pipeline {

struct Record {
    std::string name;
    int age;
    double salary;
};

using Records = std::vector<Record>;

Records filter_adults(Records rs) {
    std::cout << "  [Filter] Remove minors\n";
    rs.erase(std::remove_if(rs.begin(), rs.end(),
        [](const Record& r) { return r.age < 18; }), rs.end());
    return rs;
}

Records sort_by_salary(Records rs) {
    std::cout << "  [Filter] Sort by salary desc\n";
    std::sort(rs.begin(), rs.end(),
        [](const Record& a, const Record& b) { return a.salary > b.salary; });
    return rs;
}

Records apply_bonus(Records rs) {
    std::cout << "  [Filter] Apply 10% bonus\n";
    for (auto& r : rs) r.salary *= 1.1;
    return rs;
}

Records top_n(Records rs) {
    std::cout << "  [Filter] Take top 3\n";
    if (rs.size() > 3) rs.resize(3);
    return rs;
}

void demo() {
    Pipeline<Records> pipeline;
    pipeline
        .add(filter_adults)
        .add(apply_bonus)
        .add(sort_by_salary)
        .add(top_n);

    Records data = {
        {"Alice", 30, 80000}, {"Bob", 16, 20000},
        {"Charlie", 45, 120000}, {"Diana", 28, 95000},
        {"Eve", 22, 65000}, {"Frank", 50, 110000},
    };

    std::cout << "\n=== Data Pipeline ===\n";
    auto result = pipeline.process(data);

    std::cout << "  Result:\n";
    for (const auto& r : result)
        std::cout << "    " << r.name << " (age " << r.age
                  << ", salary $" << r.salary << ")\n";
}

} // namespace data_pipeline

int main() {
    text_pipeline::demo();
    data_pipeline::demo();
    return 0;
}
