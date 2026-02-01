#include <iostream>
#include <stlish/vector.hpp>
#include <string>

int main() {
    std::cout << "--- stlish::vector Basics Demo ---" << std::endl;

    stlish::vector<int> v1 = {10, 20, 30, 40, 50}; // Initializer list
    stlish::vector<std::string> v2;                // Default constructor

    v2.push_back("Hello");
    v2.emplace_back("World"); 

    std::cout << "v1[2]: " << v1[2] << std::endl; 
    try {
        std::cout << "v1.at(10): " << v1.at(10) << std::endl;
    } catch (const std::out_of_range& e) {
        std::cout << "Caught expected error: " << e.what() << std::endl;
    }

    std::cout << "Vector v1 elements: ";
    for (const auto& x : v1) {
        std::cout << x << " ";
    }
    std::cout << "\nSize: " << v1.size() << ", Capacity: " << v1.capacity() << std::endl;

    return 0;
}