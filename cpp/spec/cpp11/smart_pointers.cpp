#include <iostream>
#include <memory>

class MyClass {
public:
    MyClass() { std::cout << "MyClass constructed" << std::endl; }
    ~MyClass() { std::cout << "MyClass destructed" << std::endl; }
    void greet() { std::cout << "Hello from MyClass" << std::endl; }
};

int main() {
    // std::unique_ptr
    {
        std::unique_ptr<MyClass> u_ptr = std::make_unique<MyClass>();
        u_ptr->greet();
        // std::unique_ptr<MyClass> u_ptr2 = u_ptr; // Error: unique_ptr cannot be copied
    } // u_ptr goes out of scope, MyClass is destructed

    std::cout << "---" << std::endl;

    // std::shared_ptr
    {
        std::shared_ptr<MyClass> s_ptr1;
        {
            std::shared_ptr<MyClass> s_ptr2 = std::make_shared<MyClass>();
            s_ptr1 = s_ptr2;
            std::cout << "Use count: " << s_ptr1.use_count() << std::endl;
        } // s_ptr2 goes out of scope, but MyClass is not destructed
        std::cout << "Use count: " << s_ptr1.use_count() << std::endl;
    } // s_ptr1 goes out of scope, MyClass is destructed

    return 0;
}
