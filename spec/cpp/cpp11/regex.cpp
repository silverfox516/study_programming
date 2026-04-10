// <regex> — C++11
// What: regular expressions in the standard library: match, search, replace.
// Why:  before C++11 you needed Boost.Regex or PCRE; now it is portable.
// Pre:  hand-written parsing or third-party libraries.

#include <iostream>
#include <regex>
#include <string>

int main() {
    std::string text = "Contact: alice@example.com, bob@test.org, not-an-email";

    // regex_match — does the entire string match?
    std::regex email_re(R"([\w.+-]+@[\w-]+\.[\w.-]+)");
    std::cout << "match alice@example.com? "
              << std::regex_match(std::string("alice@example.com"), email_re) << "\n";

    // regex_search — does any substring match?
    std::smatch m;
    std::string remaining = text;
    std::cout << "found emails:\n";
    while (std::regex_search(remaining, m, email_re)) {
        std::cout << "  " << m.str() << "\n";
        remaining = m.suffix().str();
    }

    // regex_replace — substitute every match
    std::string redacted = std::regex_replace(text, email_re, "[REDACTED]");
    std::cout << "redacted: " << redacted << "\n";

    // Capture groups
    std::regex date_re(R"((\d{4})-(\d{2})-(\d{2}))");
    std::string date = "today is 2026-04-10 here";
    if (std::regex_search(date, m, date_re)) {
        std::cout << "year=" << m[1] << " month=" << m[2] << " day=" << m[3] << "\n";
    }

    return 0;
}
