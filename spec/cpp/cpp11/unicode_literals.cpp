// Unicode string and char literals — C++11
// What: u8"...", u"...", U"..." literals produce char/char16_t/char32_t arrays
//       with a guaranteed encoding (UTF-8/UTF-16/UTF-32).
// Why:  before C++11 there was no portable way to say "this string is UTF-16".
// Pre:  encoding was implementation-defined, with all the surprises that implies.

#include <iostream>
#include <string>

int main() {
    // UTF-8: just a sequence of bytes (char), but encoded as UTF-8 by the compiler
    const char*     u8s = u8"héllo";
    // UTF-16: char16_t array
    const char16_t* u16 = u"héllo";
    // UTF-32: char32_t array — one element per code point
    const char32_t* u32 = U"héllo";

    std::cout << "u8 (printed as bytes): " << u8s << "\n";

    // Length comparisons make the encoding obvious
    std::u16string s16(u16);
    std::u32string s32(u32);
    std::cout << "u16 length (code units): "  << s16.size() << "\n";
    std::cout << "u32 length (code points): " << s32.size() << "\n";

    // Single-character literals
    char16_t c16 = u'A';
    char32_t c32 = U'\u00e9';   // é
    std::cout << "u16 'A' value:    " << static_cast<int>(c16) << "\n";
    std::cout << "u32 'é' code pt:  " << static_cast<int>(c32) << "\n";

    return 0;
}
