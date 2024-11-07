#include <algorithm>
#include <execution>
#include <iostream>
#include <ranges>
#include <thread>

constexpr int count = 1000000000;

int main(int argc, char* argv[]) {
  std::cout << count << std::endl;
  std::jthread jth([]() { std::cout << "Hello" << std::endl; });
  jth.join();

  std::vector<double> a(count);
  std::vector<double> b(count);

  for (size_t i = 0; i < count; i++) {
    a[i] = static_cast<double>(i);
    b[i] = static_cast<double>(i) * 2.0;
  }

  std::cout << "Starting!" << std::endl;

  std::vector<double> c(count);
  auto iota = std::ranges::iota_view(0, count);

  std::for_each(std::execution::par_unseq, iota.begin(), iota.end(),
                [&a, &b, &c](int i) { c[i] = a[i] + b[i]; });

  std::cout << "Ended" << std::endl;
  return 0;
}
