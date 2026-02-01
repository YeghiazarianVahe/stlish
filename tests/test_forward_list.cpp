#include <catch2/catch_test_macros.hpp>
#include "stlish/forward_list.hpp"
#include <string>

TEST_CASE("forward_list - default construction", "[forward_list]") {
    stlish::forward_list<int> lst;
    REQUIRE(lst.empty());
}

TEST_CASE("forward_list - push_front and front", "[forward_list]") {
    stlish::forward_list<int> lst;
    
    lst.push_front(10);
    REQUIRE(!lst.empty());
    REQUIRE(lst.front() == 10);
    
    lst.push_front(20);
    REQUIRE(lst.front() == 20);
    
    lst.push_front(30);
    REQUIRE(lst.front() == 30);
}

TEST_CASE("forward_list - pop_front", "[forward_list]") {
    stlish::forward_list<int> lst;
    lst.push_front(10);
    lst.push_front(20);
    lst.push_front(30);
    
    lst.pop_front();
    REQUIRE(lst.front() == 20);
    
    lst.pop_front();
    REQUIRE(lst.front() == 10);
    
    lst.pop_front();
    REQUIRE(lst.empty());
}

TEST_CASE("forward_list - clear", "[forward_list]") {
    stlish::forward_list<int> lst;
    for (int i = 0; i < 100; ++i) {
        lst.push_front(i);
    }
    
    lst.clear();
    REQUIRE(lst.empty());
}

TEST_CASE("forward_list - move semantics", "[forward_list]") {
    stlish::forward_list<std::string> lst;
    
    std::string s = "Hello";
    lst.push_front(std::move(s));
    
    REQUIRE(lst.front() == "Hello");
    REQUIRE(s.empty());  // s was moved from
}

TEST_CASE("forward_list - destructor cleanup", "[forward_list]") {
    stlish::forward_list<int> lst;
    for (int i = 0; i < 10000; ++i) {
        lst.push_front(i);
    }
    // Destructor will run here
}

TEST_CASE("forward_list - const correctness", "[forward_list]") {
    stlish::forward_list<int> lst;
    lst.push_front(42);
    
    const auto& const_lst = lst;
    REQUIRE(const_lst.front() == 42);
    REQUIRE(!const_lst.empty());
}