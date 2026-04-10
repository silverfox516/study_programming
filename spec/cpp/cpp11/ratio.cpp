// <ratio> — C++11
// What: compile-time rational numbers expressed as a type: std::ratio<Num, Den>.
// Why:  the foundation that lets <chrono> express "milliseconds = ratio<1, 1000> seconds"
//       at compile time, with arithmetic and unit conversion checked by the type system.
// Pre:  unit conversion factors lived in floats and were checked at runtime, if at all.

#include <iostream>
#include <ratio>
#include <chrono>

int main() {
    // Predefined SI prefixes
    std::cout << "kilo: " << std::kilo::num << "/" << std::kilo::den << "\n";
    std::cout << "milli:" << std::milli::num << "/" << std::milli::den << "\n";

    // Custom ratio
    typedef std::ratio<22, 7> approx_pi;
    std::cout << "22/7 ≈ " << approx_pi::num << "/" << approx_pi::den
              << " = " << static_cast<double>(approx_pi::num) / approx_pi::den << "\n";

    // Compile-time arithmetic on ratios
    typedef std::ratio_add<std::ratio<1, 2>, std::ratio<1, 3> > five_sixths;
    std::cout << "1/2 + 1/3 = " << five_sixths::num << "/" << five_sixths::den << "\n";

    // chrono uses ratios under the hood
    typedef std::chrono::duration<double, std::milli> ms_d;
    ms_d half_second(500.0);
    std::cout << "500 ms in seconds = "
              << std::chrono::duration_cast<std::chrono::duration<double> >(half_second).count()
              << "\n";

    return 0;
}
