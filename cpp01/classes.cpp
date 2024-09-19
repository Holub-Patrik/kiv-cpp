#include "classes.hpp"
#include <iostream>

void Base::Hello() {
    std::cout << "Hello from base" << std::endl;
}

void Base::Goodbye() {
    std::cout << "Goodbye from base" << std::endl;
}

void Child::Hello() {
    std::cout << "Hello from child" << std::endl;
}

void Child::Goodbye() {
    std::cout << "Goodbye from child" << std::endl;
}
