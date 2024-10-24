// vim: softtabstop=0 shiftwidth=2 noexpandtab
#include <iostream>

template <typename T> void increment(T &val) { val++; }

int main() {
  std::cout << "Starting ... " << std::endl;

  std::string str("Hello");
  increment(str);

  std::cout << "Ending ..." << std::endl;
}
