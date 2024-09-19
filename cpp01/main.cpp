#include <iostream>
#include <vector>

constexpr int MULTIPLIER = 2;

void print_arr(std::vector<int> &arr) {
  for (int i = 0; i < arr.size(); i++) {
    std::cout << arr[i] << " ";
  }
  std::cout << std::endl;
}

void change_arr(std::vector<int> &arr) {
  for (int i = 0; i < arr.size(); i++) {
    arr[i] *= MULTIPLIER;
  }
}

int main(int argc, char **argv) {
  std::vector<int> pole = {5, 6, 42};

  print_arr(pole);
  change_arr(pole);
  print_arr(pole);

  return 0;
}
