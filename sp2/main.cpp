#include <array>
#include <cstdint>
#include <limits>
#include <print>

template <typename T>
static consteval T template_max(const T& lhs, const T& rhs) {
  if (lhs > rhs) {
    return lhs;
  } else {
    return rhs;
  }
}

template <uint64_t max_bytes> class BigNumber {
private:
  // this means I get 4 bytes for each position
  // all operations I need to implement can be done per parts
  // also dividing by 10 can be done per parts (for converting into decimal)

public:
  std::array<uint32_t, max_bytes / 4> repr;
  const uint64_t size = max_bytes;
  // default without constructor
  BigNumber() : repr(std::array<uint32_t, max_bytes / 4>{}) {
    // std::println("Array length: {}", repr.size());
  }
  // initialize given an uniternal represantation to be used
  BigNumber(std::array<uint32_t, max_bytes / 4> init_arr) : repr(init_arr) {
    // std::println("Array length: {}", repr.size());
  }
  // initialize using a number
  // allow only for initializing using uint32_t
  BigNumber(uint32_t num) : repr(std::array<uint32_t, max_bytes / 4>{}) {
    repr[0] = num;
    // std::println("Array length: {}", repr.size());
  }

  ~BigNumber() {}

  template <uint64_t other_size>
  BigNumber<template_max(other_size, max_bytes)>
  operator+(BigNumber<other_size> const& other) {
    const uint64_t resulting_size = template_max(other_size, max_bytes);

    std::array<uint32_t, resulting_size / 4> res_arr{};
    int elem_index = 0;
    uint64_t carry = 0;

    auto elem_iter = res_arr.begin();
    auto other_iter = other.repr.begin();
    auto this_iter = repr.begin();

    while (elem_iter != res_arr.end()) {
      uint64_t other_num;
      if (other_iter != other.repr.end()) {
        other_num = *other_iter;
      } else {
        other_num = 0;
      }

      uint64_t this_num;
      if (this_iter != repr.end()) {
        this_num = *this_iter;
      } else {
        this_num = 0;
      }

      uint64_t temp = this_num + other_num + carry;

      if (temp > std::numeric_limits<uint32_t>::max()) {
        carry = temp >> 32;
        *elem_iter = static_cast<uint32_t>(temp);
      } else {
        *elem_iter = static_cast<uint32_t>(temp);
        carry = 0;
      }

      elem_iter++;
      other_iter++;
      this_iter++;
    }

    return BigNumber<resulting_size>(res_arr);
  }
};

int main(int argc, char* argv[]) {
  BigNumber<12> bg1{std::numeric_limits<uint32_t>::max()};
  BigNumber<16> bg2{std::numeric_limits<uint32_t>::max()};

  auto bg3 = bg1 + bg2 + bg2 + bg2;
  std::println("\n{}\n", bg3.size);
  for (auto& elem : bg3.repr) {
    std::println("{}", elem);
  }
  return 0;
}
