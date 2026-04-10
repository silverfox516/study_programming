// <chrono> calendar — C++20
// What: year/month/day, weekday, year_month_day, and a literal-style constructor:
//       2026y/April/10. Plus formatting support via std::format.
// Why:  doing date arithmetic with system_clock + time_t is painful and error-prone.
// Pre:  ad-hoc tm_struct juggling, or external libraries like Howard Hinnant's date.

#include <iostream>
#include <chrono>
#include <format>

int main() {
    using namespace std::chrono;

    // Construct a date with the literal-style operators
    year_month_day today = 2026y/April/10;
    std::cout << "year:  " << static_cast<int>(today.year())          << "\n";
    std::cout << "month: " << static_cast<unsigned>(today.month())    << "\n";
    std::cout << "day:   " << static_cast<unsigned>(today.day())      << "\n";

    // Validity check
    year_month_day bad = 2026y/February/30;
    std::cout << "Feb 30 valid? " << bad.ok() << "\n";

    // Convert to a sys_days for arithmetic
    sys_days today_sys = today;
    sys_days week_later = today_sys + days(7);
    year_month_day next_week = week_later;
    std::cout << "next week day = " << static_cast<unsigned>(next_week.day()) << "\n";

    // Weekday from a date
    weekday wd = weekday(today_sys);
    std::cout << "weekday index: " << wd.c_encoding() << "\n";

    // std::format integration
    std::cout << std::format("formatted: {:%Y-%m-%d}\n", today_sys);

    return 0;
}
