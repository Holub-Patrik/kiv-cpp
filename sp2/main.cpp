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
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <utility>

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

template <typename T>
concept TrivialIntegral =
    std::integral<T> && requires(T t) { t >= 0 && t < 10; };

template <size_t size> class BigNumberException : std::exception {
public:
  explicit BigNumberException(const char* reason, BigInt<size> num)
      : _msg(reason), _ {}
  explicit BigNumberException(const std::string reason) : _msg(reason) {}

  virtual ~BigNumberException() noexcept {}

  virtual const char* what() const noexcept { return _msg.c_str(); }

private:
  std::string _msg;
  BigInt<size> _int;
};

template <size_t size> class BigInt {
private:
  bool negative = false;
  const size_t order = size;
  std::array<short, size> repr{};

public:
  // default constructor
  BigInt() {}
  ~BigInt() {
    // std::println("destroyed");
  }

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
  BigInt(BigInt<size>&& other) noexcept
      : repr(std::move(other.repr)), negative(other.negative) {}

  // copy constructor
  BigInt(const BigInt<size>& other_num) {
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

  constexpr const std::size_t num_order() const noexcept { return order; };
  const std::array<short, size>& get_repr() const noexcept { return repr; }
  const bool is_negative() const { return negative; }

  template <uint64_t other_size>
  BigInt<template_max(size, other_size)> operator+=(BigInt<other_size> rhs);
  template <uint64_t other_size>
  BigInt<template_max(size, other_size)> operator-=(BigInt<other_size> rhs);
  BigInt<size> operator-();

  operator std::string() const {
    std::ostringstream s;
    if (negative) {
      s << "-";
    }
    bool filler = true;

    std::for_each(repr.rbegin(), repr.rend(), [&s, &filler](auto elem) {
      if (elem != 0 || !filler) {
        s << elem;
        s << " ";
        filler = false;
      }
    });

    if (filler) {
      s << "0";
    }

    return std::string{s.str()};
  }

  const short operator[](size_t index) const { return repr[index]; }
  short& operator[](size_t index) { return repr[index]; }

  const short last() const { return *repr.rbegin(); }
  BigInt<size> operator<<(size_t shift_amount) {
    for (size_t i = size - 1; i >= shift_amount; --i) {
      if (repr[i]) {
        throw BigNumberException("Overflow");
      }
      repr[i] = repr[i - shift_amount];
    }
    for (size_t i = 0; i < shift_amount; ++i) {
      repr[i] = 0;
    }
    return *this;
  }
};

template <size_t size>
template <size_t other_size>
BigInt<template_max(size, other_size)>
BigInt<size>::operator+=(BigInt<other_size> rhs) {
  return *this + rhs;
}

template <size_t size>
template <size_t other_size>
BigInt<template_max(size, other_size)>
BigInt<size>::operator-=(BigInt<other_size> rhs) {
  return *this - rhs;
}

template <size_t lhs_size, size_t rhs_size>
BigInt<template_max(lhs_size, rhs_size)>
operator+(const BigInt<lhs_size>& lhs, const BigInt<rhs_size>& rhs) {
  constexpr const size_t resulting_size = template_max(lhs_size, rhs_size);

  // addition implementation works only for positive numbers
  // so I just transform everything into addition/subtraction of positive
  // numbers
  const bool lhs_neg = lhs.is_negative();
  const bool rhs_neg = rhs.is_negative();
  const auto lr_cmp = lhs <=> rhs;

  if (!lhs_neg && rhs_neg) {
    return lhs - -rhs;
  }

  if (lhs_neg && !rhs_neg) {
    return -(-lhs - rhs);
  }

  if (lhs_neg && rhs_neg) {
    return -(-lhs + -rhs);
  }

  std::array<short, resulting_size> res_arr;
  short carry = 0;
  auto index_gen = std::views::iota(0);
  auto index = std::views::counted(index_gen.begin(), resulting_size);

  std::for_each(index.begin(), index.end(),
                [&carry, &res_arr, &rhs, &lhs](auto i) {
                  const short lhs_num = i < lhs_size ? lhs[i] : 0;
                  const short rhs_num = i < rhs_size ? rhs[i] : 0;
                  const short temp = lhs_num + rhs_num + carry;
                  res_arr[i] = temp % 10;
                  carry = temp / 10;
                });

  if (carry) {
    throw BigNumberException("Overflow");
  }

  return BigInt<resulting_size>{std::move(res_arr)};
}

template <size_t lhs_size, size_t rhs_size>
BigInt<template_max(lhs_size, rhs_size)>
operator-(const BigInt<lhs_size>& lhs, const BigInt<rhs_size>& rhs) {
  constexpr const size_t resulting_size = template_max(lhs_size, rhs_size);

  // subtraction implementation works only for positive numbers
  // so I created these rules to convert everything into addition/subtraction of
  // positive numbers
  const bool lhs_neg = lhs.is_negative();
  const bool rhs_neg = rhs.is_negative();
  const auto lr_cmp = lhs <=> rhs;

  if ((lhs <=> rhs) == 0) {
    return BigInt<resulting_size>{0};
  }

  if (!lhs_neg && !rhs_neg) {
    if ((lr_cmp) < 0) {
      return -(rhs - lhs);
    }
  }

  if (!lhs_neg && rhs_neg) {
    return lhs + -rhs;
  }

  if (lhs_neg && !rhs_neg) {
    return -(-lhs + rhs);
  }

  if (lhs_neg && rhs_neg) {
    return -(-lhs - -rhs);
  }

  std::array<short, resulting_size> res_arr;
  short carry = 0;
  for (size_t i = 0; i < resulting_size; ++i) {
    short lhs_num = i < lhs_size ? lhs[i] : 0;
    short rhs_num = i < rhs_size ? rhs[i] + carry : 0;
    if (lhs_num - rhs_num >= 0) {
      res_arr[i] = lhs_num - rhs_num;
      carry = 0;
    } else {
      res_arr[i] = (lhs_num + 10) - rhs_num;
      carry = 1;
    }
  }

  return BigInt<resulting_size>{std::move(res_arr)};
}

template <size_t lhs_size, size_t rhs_size>
BigInt<template_max(lhs_size, rhs_size)>
operator*(const BigInt<lhs_size>& lhs, const BigInt<rhs_size>& rhs) {}

template <size_t size>
BigInt<size> operator*(const BigInt<size>& num,
                       const TrivialIntegral auto mul) {
  std::array<short, size> res_arr;
  short carry = 0;
  auto index_gen = std::views::iota(0);
  auto index = std::views::counted(index_gen.begin(), size);

  std::for_each(index.begin(), index.end(),
                [&carry, &res_arr, &num, &mul](auto i) {
                  const short temp = num[i] * mul + carry;
                  res_arr[i] = temp % 10;
                  carry = temp / 10;
                });
  if (carry) {
    throw BigNumberException("Overflow");
  }

  return BigInt<size>{std::move(res_arr)};
}

template <size_t lhs_size, size_t rhs_size>
auto operator<=>(const BigInt<lhs_size>& lhs, const BigInt<rhs_size>& rhs) {
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
    bool order_set = false;
    int i; // size_t here breaks the loop
    for (i = lhs_index; i >= 0; --i) {
      if (lhs[i] != rhs[i]) {
        order = lhs[i] <=> rhs[i];
        order_set = true;
        break;
      }
    }

    if (!order_set) {
      return std::strong_ordering::equal;
    }
  }

  // if the numbers were negative, the results needs to be flipped
  if (lhs.is_negative()) {
    order = 0 <=> order;
  }

  return order;
}

template <size_t size> BigInt<size> BigInt<size>::operator-() const {
  auto new_num = BigInt<size>(*this);
  new_num.negative = !new_num.negative;
  return new_num;
}

void print_sums() {
  BigInt<10> pos_a{89};
  BigInt<11> pos_b{18};
  BigInt<12> neg_a{-13};
  BigInt<13> neg_b{-14};

  std::println("a({}) - a({}) = {}", pos_a.to_string(), pos_a.to_string(),
               (pos_a - pos_a).to_string());

  // a + b => a + b;
  std::println("a({}) + b({}) = {}", pos_a.to_string(), pos_b.to_string(),
               (pos_a + pos_b).to_string());

  // a - b => -(b - a)
  std::println("a({}) - b({}) = {}", pos_a.to_string(), pos_b.to_string(),
               (pos_a - pos_b).to_string());

  // a - b => a - b
  std::println("a({}) - b({}) = {}", pos_b.to_string(), pos_a.to_string(),
               (pos_b - pos_a).to_string());

  // a + (-b) => a - b
  std::println("a({}) + b({}) = {}", pos_a.to_string(), neg_b.to_string(),
               (pos_a + neg_b).to_string());

  // a - (-b) => a + b
  std::println("a({}) - b({}) = {}", pos_a.to_string(), neg_b.to_string(),
               (pos_a - neg_b).to_string());

  // -a + b => -(a - b)
  std::println("a({}) + b({}) = {}", neg_a.to_string(), pos_b.to_string(),
               (neg_a + pos_b).to_string());

  // -a - b => -(a + b)
  std::println("a({}) - b({}) = {}", neg_a.to_string(), pos_b.to_string(),
               (neg_a - pos_b).to_string());

  // -a  + (-b) => -(a + b)
  std::println("a({}) + b({}) = {}", neg_a.to_string(), neg_b.to_string(),
               (neg_a + neg_b).to_string());

  // -a - (-b) => -(a - b)
  std::println("a({}) - b({}) = {}", neg_a.to_string(), neg_b.to_string(),
               (neg_a - neg_b).to_string());
}

int main(int argc, char* argv[]) {
  print_sums();
  BigInt<3> a{500};
  BigInt<3> b{500};
  std::println("{}", (a << 3).to_string());
  try {
    auto c = b + b;
    std::println("{}", c.to_string());
  } catch (BigNumberException e) {
    std::println("{}", e.what());
  }
  // print_sums();
  return 0;
}
