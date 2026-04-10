#include <iostream>
#include <functional>
#include <string>

void print_greeting(const std::string& prefix, const std::string& name, const std::string& suffix) {
    std::cout << prefix << " " << name << suffix << std::endl;
}

int main() {
    // std::bind_back binds the last N arguments.
    // Standardized in C++23.
    auto greet_with_exclamation = std::bind_back(print_greeting, "!");
    
    // Now we just provide the first two arguments
    greet_with_exclamation("Hello", "World");
    greet_with_exclamation("Hi", "Alice");

    auto say_hello_to = std::bind_back(print_greeting, "World", "!");
    say_hello_to("Hello"); 

    return 0;
}
