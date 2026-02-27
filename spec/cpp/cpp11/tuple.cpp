#include <iostream>
#include <tuple>
#include <string>

std::tuple<int, std::string, double> get_student_info() {
    return std::make_tuple(12345, "John Doe", 3.8);
}

int main() {
    auto student = get_student_info();

    std::cout << "ID: " << std::get<0>(student) << std::endl;
    std::cout << "Name: " << std::get<1>(student) << std::endl;
    std::cout << "GPA: " << std::get<2>(student) << std::endl;

    // std::tie to unpack
    int id;
    std::string name;
    double gpa;
    std::tie(id, name, gpa) = get_student_info();

    std::cout << "Unpacked - ID: " << id << ", Name: " << name << ", GPA: " << gpa << std::endl;

    return 0;
}
