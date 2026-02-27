#include <iostream>
#include <source_location>

void log(const char* message, const std::source_location& location = std::source_location::current()) {
    std::cout << "Log: " << message << std::endl;
    std::cout << "  File: " << location.file_name() << std::endl;
    std::cout << "  Func: " << location.function_name() << std::endl;
    std::cout << "  Line: " << location.line() << std::endl;
}

int main() {
    log("Hello World");
    return 0;
}
