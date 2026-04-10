// Raw and prefixed string literals — C++11
// What: R"(...)" disables backslash escaping; u8/u/U/L prefixes pick the encoding.
// Why:  regexes and Windows paths drown in backslashes; raw literals fix that.
// Pre:  every backslash had to be doubled, which made regex unreadable.

#include <iostream>
#include <string>

int main() {
    // Old way: every backslash escaped twice
    std::string old_path = "C:\\Users\\alice\\file.txt";
    std::cout << "escaped: " << old_path << "\n";

    // Raw literal: write what you mean
    std::string raw_path = R"(C:\Users\alice\file.txt)";
    std::cout << "raw:     " << raw_path << "\n";

    // Custom delimiter for content that contains )"
    std::string code = R"cpp(printf("hello\n"); return 0;)cpp";
    std::cout << "code:    " << code << "\n";

    // Multi-line raw literals preserve newlines literally
    std::string sql = R"(
        SELECT id, name
        FROM users
        WHERE active = 1;
    )";
    std::cout << "sql block:" << sql << "\n";

    // Encoding prefixes
    const char     ascii[]  =  "ascii";
    const char16_t utf16[]  = u"utf-16";
    const char32_t utf32[]  = U"utf-32";
    const wchar_t  wide []  = L"wide";
    (void)ascii; (void)utf16; (void)utf32; (void)wide;

    // u8 selects UTF-8 encoding for the source -> object representation
    const char* utf8 = u8"utf-8 こんにちは";
    std::cout << "utf-8 bytes: " << utf8 << "\n";

    return 0;
}
