#include "Number.hpp"
#include "NumberDef.hpp"
#include "NumberEx.hpp"
#include "Term.hpp"
#include <exception>
#include <iostream>
#include <print>

void print_sums() {
  std::cout << "-- Limited --" << std::endl;
  {
    MP::Num<10> pos_a{21};
    MP::Num<11> pos_b{19};
    MP::Num<12> neg_a{-13};
    MP::Num<13> neg_b{-14};
    std::cout << pos_a << " -= " << pos_b << " => " << (pos_a -= pos_b)
              << std::endl;
    std::cout << pos_a << " += " << pos_b << " => " << (pos_a += pos_b)
              << std::endl;
    auto c = MP::Num<10>{2543} / MP::Num<10>{10};
    std::cout << c << std::endl;

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
    try {
      std::cout << (a << 3) << std::endl;
    } catch (MP::Exception<3> er) {
      std::cout << "correct" << std::endl;
    }
    std::cout << "-- exception handling ---" << std::endl;
    try {
      auto c = b + b;
      std::cout << c << std::endl;
    } catch (std::exception& e) {
      std::println("{}", e.what());
    }
  }

  std::cout << "-- +/+= --" << std::endl;
  {
    MP::Num<10> a{89};
    MP::Num<5> b{18};
    std::cout << "-- += --" << std::endl;
    a += b;
    std::cout << a << std::endl;
    std::cout << "-- -= --" << std::endl;
    a -= b;
    std::cout << a << std::endl;
  }

  std::cout << "-- * --" << std::endl;
  {
    MP::Num<10> a{420};
    MP::Num<10> b{-69};
    auto c = a * b;
    std::cout << "a * b = " << c << std::endl;
  }

  std::cout << "-- *= --" << std::endl;
  {
    MP::Num<10> a{420};
    MP::Num<10> b{-69};
    std::cout << "a *= b = " << (a *= b) << std::endl;
  }

  std::cout << "-- / --" << std::endl;
  {
    MP::Num<15> a{78654};
    MP::Num<10> b{573};

    std::cout << "a / b = " << a / b << std::endl;
  }

  std::cout << "-- /= --" << std::endl;
  {
    MP::Num<15> a{-78654};
    MP::Num<10> b{573};

    std::cout << "a /= b | a = " << (a /= b) << std::endl;
  }

  std::cout << "-- ! --" << std::endl;
  {
    MP::Num<10> a{10};
    std::cout << a.factorial() << std::endl;
  }
}

int main(int argc, char* argv[]) {
  // std::cout << "--- Addition and subtraction conversions ---" << std::endl;
  // print_sums();

  std::cout << "--- Various other tests on static ---" << std::endl;
  print_tests();

  std::cout << "--- Unlimited (Prepare for trouble) ---" << std::endl;
  MP::Num<MP::Unlimited> a{163};
  std::cout << a << std::endl;
  MP::Num<MP::Unlimited> b{292};
  std::cout << b << std::endl;
  MP::Num<MP::Unlimited> c;
  c = a + b;
  std::cout << c << std::endl;
  c = a - b; // wrong seems, that the carry is getting ignored
  std::cout << c << std::endl;
  c = b - a;
  std::cout << c << std::endl;

  { // limited test works, interesting
    std::cout << "-- quick limited test --" << std::endl;
    MP::Num<10> test_1{163};
    MP::Num<10> test_2{292};
    std::cout << test_1 * test_2 << std::endl;
    std::cout << test_2 * test_1 << std::endl;
    std::cout << test_1 / test_2 << std::endl;
    std::cout << test_2 / test_1 << std::endl;
    std::cout << "-- end --" << std::endl;
  }

  // funnily enough, both are wrong
  c = a * b; // different result from b*a
  std::cout << c << std::endl;
  c = b * a;
  std::cout << c << std::endl;
  c = a / b;
  std::cout << c << std::endl;
  c = b / a;
  std::cout << c << std::endl;

  // std::cout << "--- string instatiation ---" << std::endl;

  // const char* number_inter = "3458239";
  // const char* number_inter_neg = "-3458239";
  // MP::Num<10> num_char_l{number_inter};
  // MP::Num<MP::Unlimited> num_char_u{number_inter};
  // MP::Num<10> num_char_l_neg{number_inter_neg};
  // MP::Num<MP::Unlimited> num_char_u_neg{number_inter_neg};

  // std::cout << "Char* :" << number_inter << std::endl;
  // std::cout << "Char* Limited: " << num_char_l << std::endl;
  // std::cout << "Char* Unlimited: " << num_char_u << std::endl;
  // std::cout << "Char* Limited Neg: " << num_char_l_neg << std::endl;
  // std::cout << "Char* Unlimited Neg: " << num_char_u_neg << std::endl;

  // const std::string number_string = "7564849";
  // const std::string number_string_neg = "-7564849";
  // MP::Num<10> num_string_l{number_string};
  // MP::Num<MP::Unlimited> num_string_u{number_string};
  // MP::Num<10> num_string_l_neg{number_string_neg};
  // MP::Num<MP::Unlimited> num_string_u_neg{number_string_neg};
  // std::cout << "String :" << number_string << std::endl;
  // std::cout << "String Limited:" << num_string_l << std::endl;
  // std::cout << "String Unlimited:" << num_string_u << std::endl;
  // std::cout << "String Limited Neg:" << num_string_l_neg << std::endl;
  // std::cout << "String Unlimited Neg:" << num_string_u_neg << std::endl;

  // Term<MP::Unlimited> t{};
  // t.run();

  MP::Num<MP::Unlimited> err{10};
  c = err.factorial();
  std::cout << c << std::endl;

  return 0;
}
