// hello.cppm
// To compile: clang++ -std=c++20 --precompile -o hello.pcm hello.cppm
export module hello;

import <iostream>;

export void say_hello() {
    std::cout << "Hello, Modules!" << std::endl;
}
