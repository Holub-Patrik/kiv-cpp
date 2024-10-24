#include <iostream>

class Test {
    private:
    int piss;
    public:
    Test(int given) : piss(given){}
    ~Test() {
        std::cout << "Goblin" << std::endl;
    }

    void print_piss() {
        std::cout << piss << std::endl;
    }
};

int main() {
    std::cout << "Starting ... " << std::endl;
    Test t1{10};
    t1.print_piss();
    std::cout << "Ending ..." << std::endl;
}