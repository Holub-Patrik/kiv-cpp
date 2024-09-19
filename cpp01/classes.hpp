#pragma once
#include <iostream>

class Base {
    public:
    Base() {
        std::cout << "Base constructor" << std::endl;
    }
    ~Base() {
        std::cout << "Base destructor" << std::endl;
    }
virtual void Hello();
    virtual void Goodbye();
};

class Child : public Base {
    public:
    Child() {
        std::cout << "Child constructor" << std::endl;
    }
    ~Child() {
        std::cout << "Child destructor" << std::endl;
    }
    void Hello() override;
    void Goodbye() override;
};
