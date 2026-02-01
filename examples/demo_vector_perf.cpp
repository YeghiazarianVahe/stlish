#include <iostream>
#include <stlish/vector.hpp>
#include <chrono>
#include <vector>

struct HeavyObject {
    int* data;
    HeavyObject() : data(new int[1000]) {}
    ~HeavyObject() { delete[] data; }
    
    HeavyObject(const HeavyObject& other) : data(new int[1000]) {
    }
    HeavyObject(HeavyObject&& other) noexcept : data(other.data) {
        other.data = nullptr;
    }
};

int main() {
    const int N = 10000;

    stlish::vector<int> v;
    auto start = std::chrono::high_resolution_clock::now();
    
    v.reserve(N); 
    for (int i = 0; i < N; ++i) {
        v.push_back(i);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Time with reserve: " << diff.count() << "s" << std::endl;

    stlish::vector<HeavyObject> heavy_v;
    heavy_v.reserve(2);
    
    HeavyObject obj;
    heavy_v.push_back(std::move(obj));
    std::cout << "Moved object into vector efficiently." << std::endl;

    return 0;
}