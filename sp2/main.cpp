#include <algorithm>
#include <array>
#include <cstdint>
#include <execution>
#include <limits>
#include <print>
#include <ranges>

template <typename T>
static consteval T template_max(const T& lhs, const T& rhs) {
  if (lhs > rhs) {
    return lhs;
  } else {
    return rhs;
  }
}

template <uint64_t size> class BigNumber {
private:
  // this means I get 4 bytes for each position
  // all operations I need to implement can be done per parts
  // also dividing by 10 can be done per parts (for converting into decimal)
  const uint64_t bytes = size;
  std::array<uint32_t, size / 4> repr;

public:
  // default without constructor
  BigNumber() : repr(std::array<uint32_t, size / 4>{}) {
    // std::println("Array length: {}", repr.size());
  }
  // initialize given an internal represantation to be used
  BigNumber(std::array<uint32_t, size / 4> init_arr) : repr(init_arr) {
    // std::println("Array length: {}", repr.size());
  }
  // initialize using a number
  // allow only for initializing using uint32_t
  BigNumber(uint32_t num) : repr(std::array<uint32_t, size / 4>{}) {
    repr[0] = num;
    // std::println("Array length: {}", repr.size());
  }

  ~BigNumber() {}

  constexpr const uint64_t max_bytes() const noexcept { return bytes; }
  constexpr const std::array<uint32_t, size / 4>& get_repr() const noexcept {
    return repr;
  }
};

template <uint64_t lhs_size, uint64_t rhs_size>
BigNumber<template_max(lhs_size, rhs_size)>
operator+(BigNumber<lhs_size> const& lhs, BigNumber<rhs_size> const& rhs) {
  const uint64_t resulting_size = template_max(lhs_size, rhs_size);
  const auto& rhs_arr = rhs.get_repr();
  const auto& lhs_arr = lhs.get_repr();

  std::array<uint32_t, resulting_size / 4> res_arr{};
  std::array<uint32_t, resulting_size / 4> carry_arr{};
  auto index_gen = std::views::iota(0);
  auto index = std::views::counted(index_gen.begin(), resulting_size / 4);

  // copy the shorter array into the res_arr
  // the shorter is copied so I can work easily with same length one
  if (lhs_size > rhs_size) {
    std::copy(rhs_arr.begin(), rhs_arr.end(), res_arr.begin());
    auto test = *index.begin();
    std::for_each(std::execution::par_unseq, index.begin(), index.end(),
                  [&carry_arr, &res_arr, lhs_arr](auto index) {
                    const uint64_t temp =
                        static_cast<uint64_t>(res_arr[index]) +
                        static_cast<uint64_t>(lhs_arr[index]);
                    res_arr[index] = static_cast<uint32_t>(temp);
                    carry_arr[index] = temp >> 32;
                  });

  } else {
    std::copy(lhs_arr.begin(), lhs_arr.end(), res_arr.begin());

    std::for_each(std::execution::par_unseq, index.begin(), index.end(),
                  [&carry_arr, &res_arr, rhs_arr](auto index) {
                    const uint64_t temp =
                        static_cast<uint64_t>(res_arr[index]) +
                        static_cast<uint64_t>(rhs_arr[index]);
                    res_arr[index] = static_cast<uint32_t>(temp);
                    carry_arr[index] = temp >> 32;
                  });
  }

  std::for_each(std::execution::par_unseq, index.begin(), index.end(),
                [&carry_arr, &res_arr](auto index) {
                  if (index == 0) {
                    return;
                  }
                  res_arr[index] += carry_arr[index - 1];
                });

  return BigNumber<resulting_size>(res_arr);
}

int main(int argc, char* argv[]) {
  BigNumber<12> bg1{std::numeric_limits<uint32_t>::max()};
  BigNumber<16> bg2{std::numeric_limits<uint32_t>::max()};

  auto bg3 = bg1 + bg2;
  std::println("\n{}\n", bg3.max_bytes());
  for (auto& elem : bg3.get_repr()) {
    std::println("{}", elem);
  }
  return 0;
}
