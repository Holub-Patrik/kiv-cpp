#include "BigInt.hpp"
#include "BigIntDef.hpp"
// #include "BigNumberException.hpp"
#include <exception>

void print_sums() {

  BigInt<10> pos_a{89};
  BigInt<11> pos_b{18};
  BigInt<12> neg_a{-13};
  BigInt<13> neg_b{-14};

  std::cout << pos_a << " - " << pos_a << " = " << pos_a - pos_a << std::endl;

  // a + b => a + b;
  std::cout << pos_a << " + " << pos_b << " = " << pos_a + pos_b << std::endl;

  // a - b => -(b - a)
  std::cout << pos_a << " - " << pos_b << " = " << pos_a - pos_b << std::endl;

  // a - b => a - b
  std::cout << pos_b << " - " << pos_a << " = " << pos_b - pos_a << std::endl;

  // a + (-b) => a - b
  std::cout << pos_a << " + " << neg_b << " = " << pos_a + neg_b << std::endl;

  // a - (-b) => a + b
  std::cout << pos_a << " - " << neg_b << " = " << pos_a - neg_b << std::endl;

  // -a + b => -(a - b)
  std::cout << neg_a << " + " << pos_b << " = " << neg_a + pos_b << std::endl;

  // -a - b => -(a + b)
  std::cout << neg_a << " - " << pos_b << " = " << neg_a - pos_b << std::endl;

  // -a  + (-b) => -(a + b)
  std::cout << neg_a << " + " << neg_b << " = " << neg_a + neg_b << std::endl;

  // -a - (-b) => -(a - b)
  std::cout << neg_a << " - " << neg_b << " = " << neg_a - neg_b << std::endl;
}

int main(int argc, char* argv[]) {
  print_sums();
  BigInt<3> a{500};
  BigInt<3> b{500};
  std::cout << (a << 3) << std::endl;
  try {
    auto c = b + b;
    std::cout << c << std::endl;
  } catch (std::exception& e) {
    std::println("{}", e.what());
  }
  return 0;
}
