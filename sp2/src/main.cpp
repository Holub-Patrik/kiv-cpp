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
  //
  auto c = pos_a + neg_b;
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

  {
    BigInt<3> a{500};
    BigInt<3> b{500};
    std::cout << (a << 3) << std::endl;
    try {
      auto c = b + b;
      std::cout << c << std::endl;
    } catch (std::exception& e) {
      std::println("{}", e.what());
    }
  }

  {
    BigInt<10> a{89};
    BigInt<5> b{18};
    a += b;
    std::cout << a << std::endl;
    a -= b;
    std::cout << a << std::endl;
    a -= b * 2;
    std::cout << a << std::endl;
  }

  {
    BigInt<10> a{420};
    BigInt<10> b{-69};
    auto c = a * b;
    std::cout << "a * b = " << c << std::endl;
  }

  {
    BigInt<10> a{420};
    BigInt<10> b{-69};
    std::cout << "a *= b = " << (a *= b) << std::endl;
  }

  {
    BigInt<15> a{78654};
    BigInt<10> b{573};

    std::cout << "a / b = " << a / b << std::endl;
  }

  {
    BigInt<15> a{-78654};
    BigInt<10> b{573};

    std::cout << "a /= b | a = " << (a /= b) << std::endl;
  }

  {
    BigInt<1000> a{20};
    std::cout << a.factorial() << std::endl;
  }

  return 0;
}
