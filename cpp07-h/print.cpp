#include <iostream>
#include <vector>

template <typename T> std::vector<T>& operator+=(std::vector<T>& vec, T arg) {
  vec.push_back(arg);
  return vec;
}

template <typename T> std::string quote_string(T arg) {
  return std::format("\"{}\"", arg);
}

template <typename... arg> void quoted_print(arg... args) {
  std::vector<std::string> parts;
  ((parts += quote_string(args)), ...);
  for (auto it = parts.begin(); it != parts.end(); it++) {
    std::cout << *it << (it != parts.end() - 1 ? ", " : "");
  }
  std::cout << std::endl;
}

int main(int argc, char* argv[]) {
  std::cout << "Starting..." << std::endl;
  quoted_print(10, "Hello", 777.7);
  return 0;
}
