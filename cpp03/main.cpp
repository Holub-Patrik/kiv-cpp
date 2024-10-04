#include <iostream>

class Number {
private:
  double number;

public:
  Number() = default;
  explicit Number(double c) : number(c) {}
  explicit Number(double a, double b) : number(a * b) {}

  double getNumber() const { return number; }

  Number operator+(const Number &b) const {
    return Number(number + b.getNumber());
  }

  Number &operator+=(const Number &b) {
    number += b.getNumber();
    return *this;
  }
};

void print_num(Number n) {
  std::cout << "Number is: " << n.getNumber() << std::endl;
}

std::ostream &operator<<(std::ostream &os, const Number &n) {
  os << "Number is: " << n.getNumber();
  return os;
}

int main(int argc, char *argv[]) {
  Number a(10);
  Number b(15);
  Number c(25);

  a += b += c;

  std::cout << a << std::endl;
  return 0;
}
