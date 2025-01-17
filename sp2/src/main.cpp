#include "Number.hpp"
#include "NumberDef.hpp"
#include <exception>
#include <iostream>
#include <print>

void print_sums() {
  std::cout << "-- Limited --" << std::endl;
  {
    MP::Num<10> pos_a{89};
    MP::Num<11> pos_b{18};
    MP::Num<12> neg_a{-13};
    MP::Num<13> neg_b{-14};

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
  std::cout << "-- Unlimited --" << std::endl;
  {
    MP::Num<MP::Unlimited> pos_a{89};
    MP::Num<MP::Unlimited> pos_b{18};
    MP::Num<MP::Unlimited> neg_a{-13};
    MP::Num<MP::Unlimited> neg_b{-14};

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
  std::cout << "-- Unlimited(A) | Limited(B) --" << std::endl;
  {
    MP::Num<MP::Unlimited> pos_a{89};
    MP::Num<10> pos_b{18};
    MP::Num<MP::Unlimited> neg_a{-13};
    MP::Num<10> neg_b{-14};

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
  std::cout << "-- Limited(A) | Unlimited(B) --" << std::endl;
  {
    MP::Num<10> pos_a{89};
    MP::Num<MP::Unlimited> pos_b{18};
    MP::Num<10> neg_a{-13};
    MP::Num<MP::Unlimited> neg_b{-14};

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
}

void print_tests() {
  {
    MP::Num<3> a{500};
    MP::Num<3> b{500};
    std::cout << (a << 3) << std::endl;
    std::cout << "-- exception handling ---" << std::endl;
    try {
      auto c = b + b;
      std::cout << c << std::endl;
    } catch (std::exception& e) {
      std::println("{}", e.what());
    }
  }

  {
    MP::Num<10> a{89};
    MP::Num<5> b{18};
    std::cout << "-- += --" << std::endl;
    a += b;
    std::cout << a << std::endl;
    std::cout << "-- -= --" << std::endl;
    a -= b;
    std::cout << a << std::endl;
    std::cout << "-- -= b * 2 --" << std::endl;
    a -= b * 2;
    std::cout << a << std::endl;
  }

  {
    MP::Num<10> a{420};
    MP::Num<10> b{-69};
    auto c = a * b;
    std::cout << "a * b = " << c << std::endl;
  }

  {
    MP::Num<10> a{420};
    MP::Num<10> b{-69};
    std::cout << "a *= b = " << (a *= b) << std::endl;
  }

  {
    MP::Num<15> a{78654};
    MP::Num<10> b{573};

    std::cout << "a / b = " << a / b << std::endl;
  }

  {
    MP::Num<15> a{-78654};
    MP::Num<10> b{573};

    std::cout << "a /= b | a = " << (a /= b) << std::endl;
  }

  {
    MP::Num<1000> a{20};
    std::cout << a.factorial() << std::endl;
  }
}

int main(int argc, char* argv[]) {
  std::cout << "--- Addition and subtraction conversions ---" << std::endl;
  print_sums();
  std::cout << "--- Various other tests on static ---" << std::endl;
  print_tests();
  std::cout << "--- Unlimited (Prepare for trouble) ---" << std::endl;
  MP::Num<MP::Unlimited> a{100};
  std::cout << a << std::endl;
  MP::Num<MP::Unlimited> b{200};
  std::cout << b << std::endl;
  auto c = a + b;
  std::cout << c << std::endl;
  return 0;
}
