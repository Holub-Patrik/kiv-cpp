#include <algorithm>
#include <array>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <exception>
#include <execution>
#include <format>
#include <iterator>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <utility>

// the complete insanity done in operator+() will be done for muptiplication too
// the adition of the carry array will have to be done twice
// First time to fix add the carry, and then add the carries, that may have been
// generated again

template <typename T>
static consteval T template_max(const T& lhs, const T& rhs) {
  if (lhs > rhs) {
    return lhs;
  } else {
    return rhs;
  }
}

template <typename T>
concept SizeParam = std::convertible_to<T, uint64_t> && requires(T t) {
  { t + t } -> std::convertible_to<T>;
  { t - t } -> std::convertible_to<T>;
  { t % t } -> std::convertible_to<T>;
  { t / t } -> std::convertible_to<T>;
};

class BigNumberException : std::exception {
public:
  explicit BigNumberException(const char* reason) : _msg(reason) {}
  explicit BigNumberException(const std::string reason) : _msg(reason) {}

  virtual ~BigNumberException() noexcept {}

  virtual const char* what() const noexcept { return _msg.c_str(); }

private:
  std::string _msg;
};

template <size_t size> class BigInt {
private:
  bool negative = false;
  const size_t order = size;
  std::array<short, size> repr{};

public:
  // default constructor
  BigInt() {}
  ~BigInt() {}

  // initialize using a number
  // allow only for initializing using
  BigInt(long num) : repr(std::array<short, size>{}) {
    int i = 0;
    while (num >= 10) {
      repr[i] = num % 10;
      num /= 10;
      ++i;
    }

    repr[i] = num;
  }

  BigInt(std::array<short, size>&& repr) : repr(repr) {}

  // move constructor
  BigInt(BigInt<size>&& other) : repr(other.repr), negative(other.negative) {}

  // copy constructor
  BigInt(BigInt<size>& other_num) {
    negative = other_num.negative;
    repr = other_num.repr;
  }

  // copy assignment
  BigInt& operator=(const BigInt<size>& other) {
    if (this == &other)
      return *this;

    BigInt<size> temp(other);
    std::swap(repr, temp.repr);
    negative = temp.negative;

    return *this;
  }

  // move assignment
  BigInt& operator=(BigInt<size>&& other) noexcept {
    BigInt<size> temp(std::move(other));
    std::swap(repr, temp.repr);
    negative = temp.negative;
    return *this;
  }

  constexpr const uint64_t num_order() const noexcept { return order; };
  const std::array<short, size>& get_repr() const noexcept { return repr; }
  constexpr bool is_negative() const { return negative; }

  template <uint64_t other_size>
  BigInt<template_max(size, other_size)> operator+=(BigInt<other_size> rhs);

  const std::string to_string() const {
    std::ostringstream s;
    std::for_each(repr.rbegin(), repr.rend(), [&s](auto elem) { s << elem; });

    return std::string{s.str()};
  }
};

template <size_t size>
template <size_t other_size>
BigInt<template_max(size, other_size)>
BigInt<size>::operator+=(BigInt<other_size> rhs) {
  return *this + rhs;
}

template <size_t lhs_size, size_t rhs_size>
BigInt<template_max(lhs_size, rhs_size)>
operator+(const BigInt<lhs_size>& lhs, const BigInt<rhs_size>& rhs) {
  constexpr const size_t resulting_size = template_max(lhs_size, rhs_size);
  const auto& rhs_arr = rhs.get_repr();
  const auto& lhs_arr = lhs.get_repr();

  std::array<short, resulting_size> res_arr;
  std::array<short, resulting_size> carry_arr;
  auto index_gen = std::views::iota(0);
  auto index = std::views::counted(index_gen.begin(), resulting_size);

  std::for_each(std::execution::par_unseq, index.begin(), index.end(),
                [&carry_arr, &res_arr, &rhs_arr, &lhs_arr](auto index) {
                  const short lhs_num =
                      index == lhs_arr.size() ? 0 : lhs_arr[index];
                  const short rhs_num =
                      index == rhs_arr.size() ? 0 : rhs_arr[index];
                  const short temp = lhs_num + rhs_num;
                  res_arr[index] = temp % 10;
                  carry_arr[index] = temp / 10;
                });

  std::for_each(std::execution::par_unseq, index.begin() + 1, index.end(),
                [&carry_arr, &res_arr](auto index) {
                  res_arr[index] += carry_arr[index - 1];
                });

  return BigInt<resulting_size>{std::move(res_arr)};
}

template <uint64_t lhs_size, uint64_t rhs_size>
auto operator<=>(BigInt<lhs_size> const& rhs, BigInt<rhs_size> const& lhs) {
  auto lhs_arr = lhs.get_repr();
  auto rhs_arr = rhs.get_repr();
  size_t lhs_index = 0;
  size_t rhs_index = 0;

  for (auto it = lhs_arr.end(); it != lhs_arr.begin();) {
    --it;

    if (*it != 0) {
      lhs_index = std::distance(lhs_arr.begin(), it);
      break;
    }
  }

  for (auto it = rhs_arr.end(); it != rhs_arr.begin();) {
    --it;

    if (*it != 0) {
      lhs_index = std::distance(rhs_arr.begin(), it);
      break;
    }
  }

  std::println("{} ? {}", lhs_index, rhs_index);
  if (lhs_index > rhs_index) {
    return lhs_index <=> rhs_index;
  } else if (lhs_index < rhs_index) {
    return lhs_index <=> rhs_index;
  } else {
    return lhs_arr[lhs_index] <=> rhs_arr[rhs_index];
  }
}

int main(int argc, char* argv[]) {
  BigInt<4> bg1{6000};
  BigInt<5> bg2{6000};

  auto bg3 = bg1 + bg2;
  std::println("{}", bg3.to_string());
  return 0;
}
