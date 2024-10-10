#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <ranges>
#include <vector>

std::map<std::string, std::function<void()>> handlerTable;

int main(int argc, char *argv[]) {
  auto nums = std::views::iota(1, 11);

  auto even = nums | std::views::filter([](int c) { return c % 2 == 0; });

  for (auto num : even) {
    std::cout << num << std::endl;
  }

  int count_div =
      std::count_if(nums.begin(), nums.end(), [](int c) { return c % 2 == 0; });

  std::cout << "Divisible by 2: " << count_div << std::endl;

  return 0;
}
