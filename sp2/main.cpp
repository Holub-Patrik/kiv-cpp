#include <algorithm>
#include <array>
#include <cmath>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <execution>
#include <format>
#include <iostream>
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
  ~BigInt() { std::println("destroyed"); }

  // initialize using a number
  // allow only for initializing using long
  BigInt(long num) : repr(std::array<short, size>{}) {
    negative = num < 0;
    if (negative) {
      num *= -1;
    }

    for (int i = 0; num > 0; ++i) {
      if (i >= size) {
        throw BigNumberException(
            "Number too large for specified template size");
      }
      repr[i] = num % 10;
      num /= 10;
    }
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
  BigInt<size> operator-();

  const std::string to_string() const {
    std::ostringstream s;
    if (negative) {
      s << "-";
    }
    bool filler = true;

    std::for_each(repr.rbegin(), repr.rend(), [&s, &filler](auto elem) {
      if (elem != 0 || !filler) {
        s << elem;
        filler = false;
      }
    });

    return std::string{s.str()};
  }

  const short operator[](size_t index) const { return repr[index]; }
  const short last() const { return *repr.rbegin(); }
};

// currently only suports addition between positive numbers
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

  std::array<short, resulting_size> res_arr;
  std::array<short, resulting_size> carry_arr;
  auto index_gen = std::views::iota(0);
  auto index = std::views::counted(index_gen.begin(), resulting_size);

  std::for_each(std::execution::par_unseq, index.begin(), index.end(),
                [&carry_arr, &res_arr, &rhs, &lhs](auto i) {
                  const short lhs_num = i < lhs_size ? lhs[i] : 0;
                  const short rhs_num = i < rhs_size ? rhs[i] : 0;
                  const short temp = lhs_num + rhs_num;
                  res_arr[i] = temp % 10;
                  carry_arr[i] = temp / 10;
                });

  std::for_each(
      std::execution::par_unseq, index.begin() + 1, index.end(),
      [&carry_arr, &res_arr](auto i) { res_arr[i] += carry_arr[i - 1]; });

  // calculate if overflow is about to happen
  short rhs_val = *rhs.last();
  short lhs_val = *lhs.last();
  short carry_val = *carry_arr.rbegin() - 1;
  short overflow = rhs_val + lhs_val + carry_val;
  if (overflow > 10 || *carry_arr.rbegin()) {
    throw BigNumberException("Overflow");
  }

  return BigInt<resulting_size>{std::move(res_arr)};
}

template <size_t lhs_size, size_t rhs_size>
BigInt<template_max(lhs_size, rhs_size)>
operator-(const BigInt<lhs_size>& lhs, const BigInt<rhs_size>& rhs) {
  constexpr const size_t resulting_size = template_max(lhs_size, rhs_size);

  if ((lhs <=> rhs) == 0) {
    return BigInt<resulting_size>{0};
  } else if ((lhs <=> rhs) < 0) {
    return -(rhs - lhs);
  }

  const auto& rhs_arr = rhs.get_repr();
  const auto& lhs_arr = lhs.get_repr();

  std::array<short, resulting_size> res_arr;
  short carry = 0;
  for (size_t i = 0; i < resulting_size; ++i) {
    short lhs_num = i < lhs_size ? lhs[i] : 0;
    short rhs_num = i < rhs_size ? rhs[i] + carry : 0;
    std::print("{} - {} | ", lhs_num, rhs_num);
    if (lhs_num - rhs_num >= 0) {
      res_arr[i] = lhs_num - rhs_num;
      carry = 0;
    } else {
      res_arr[i] = (lhs_num + 10) - rhs_num;
      carry = 1;
    }
    std::println("{} | {}", res_arr[i], carry);
  }

  return BigInt<resulting_size>{std::move(res_arr)};
}

template <size_t lhs_size, size_t rhs_size>
auto operator<=>(BigInt<lhs_size> const& lhs, BigInt<rhs_size> const& rhs) {
  // check if one or the other is negative while the other isn't
  // this means later that checking just one is fine
  if (lhs.is_negative() && !rhs.is_negative()) {
    return std::strong_ordering::less;
  } else if (!lhs.is_negative() && rhs.is_negative()) {
    return std::strong_ordering::greater;
  }

  // find index od the largest number in the array
  size_t lhs_index = 0;
  size_t rhs_index = 0;

  for (size_t i = lhs_size; i--;) {
    if (lhs[i]) {
      lhs_index = i;
      break;
    }
  }

  for (size_t i = rhs_size; i--;) {
    if (rhs[i]) {
      rhs_index = i;
      break;
    }
  }

  // if indexes aren't equal, compare the indexes which is the same as
  // comparing which number has the greater order of magnitude
  //
  // otherwise go down the numbers to find the first non-matching
  // if no non-matching is found, return equal
  std::strong_ordering order = std::strong_ordering::less;
  if (lhs_index != rhs_index) {
    order = lhs_index <=> rhs_index;
  } else {
    size_t i;
    for (i = lhs_index; i > 0; --i) {
      if (lhs[i] != rhs[i]) {
        order = lhs[i] <=> rhs[i];
        break;
      }
    }

    if (!i) {
      return std::strong_ordering::equal;
    }
  }

  // if the numbers were negative, the results needs to be flipped
  if (lhs.is_negative()) {
    order = 0 <=> order;
  }

  return order;
}

template <size_t size> BigInt<size> BigInt<size>::operator-() {
  negative = !negative;
  return *this;
}

int main(int argc, char* argv[]) {
  BigInt<10> bg1{188};
  BigInt<20> bg2{769};

  auto bg3 = bg1 - bg2;
  std::println("{}", bg3.to_string());
  return 0;
}
