#include <cmath>
#include <iostream>
#include <print>

template <size_t Pow> double power(double number) {
  double res = number;
  for (size_t i = 0; i < Pow; i++)
    res *= number;

  return res;
}

double power(double number, size_t exp) {
  return std::pow(number, static_cast<double>(exp));
}

int main(int argc, char* argv[]) {

  size_t usr_power;
  double number;
  double res;

  while (true) {
    std::printf("Zadejde cislo: ");
    std::cin >> number;

    std::printf("Zadejte exponent: ");
    std::cin >> usr_power;

    if (usr_power == 2) {
      res = power<2>(number);
    } else if (usr_power == 3) {
      res = power<3>(number);
    } else if (usr_power == 3) {
      res = power<4>(number);
    } else if (usr_power == 3) {
      res = power<5>(number);
    } else {
      res = power(number, usr_power);
    }

    std::printf("%f", res);
  }

  return 0;
}
