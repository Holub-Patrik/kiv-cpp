#include "NumberDef.hpp"
#include "NumberEx.hpp"

#include <format>
#include <iostream>
#include <sstream>

template <int N, int M> consteval int template_max() {
  // if either one in unlimited, return should be unlimited
  if (N < 0 || M < 0) {
    return MP::Unlimited;
  } else {
    return N > M ? N : M;
  }
}

template <int N>
const MP::repr_type MP::Num<N>::operator[](size_t index) const {
  return _at(index);
}

template <int N> MP::repr_type& MP::Num<N>::operator[](size_t index) {
  return _at(index);
}

template <int N> const MP::repr_type MP::Num<N>::_at(size_t index) const {
  if (index >= repr.size()) {
    throw MP::Exception<N>("Array out of bounds access");
  }

  return repr[index];
}

template <int N> MP::repr_type& MP::Num<N>::_at(size_t index) {
  if (index >= repr.size()) {
    throw MP::Exception<N>("Array out of bounds access");
  }

  return repr[index];
}

// inline due to clang warning and auto fix
template <> inline MP::repr_type& MP::Num<MP::Unlimited>::_at(size_t index) {
  if (index >= repr.size()) {
    repr.resize(index / 10 + 10, 0);
  }

  return repr[index];
}

template <int N>
MP::Num<N>::Num(std::integral auto num) : repr(typename stl_type<N>::Type{0}) {
  negative = num < 0;
  if (negative) {
    num *= -1;
  }

  for (int i = 0; num > 0; ++i) {
    if (N >= 0 && i >= N) {
      throw MP::Exception<N>("Number too large for specified template size");
    }
    _at(i) = num % 10;
    num /= 10;
  }
}

template <int N>
MP::Num<N>::Num(const char* input_str)
    : repr(typename stl_type<N>::Type{0}), negative(false) {
  // first find \0 character, from there go in reverse
  int length = 0;
  bool is_numeric = true;
  char wrong_char = 0;

  for (int i = 0; char a = input_str[i]; i++) {
    // should be checked using a locale, but I know this won't be run somewhere
    // weird
    if (a < '0' || a > '9') {
      if (a == '-') {
        negative = true;
        length++;
        continue;
      }
      is_numeric = false;
      wrong_char = a;
      break;
    }
    length++;
  }

  if (!is_numeric) {
    throw MP::Exception<N>(
        std::format("Number given wasn't a number. Offending: {}", wrong_char));
  }

  // fix here, negative number check;
  int repr_index = 0;
  int end_cond = 0;
  if (negative) {
    end_cond = 1;
  }

  for (int i = length; i != end_cond; --i) {
    _at(repr_index) = input_str[i] - '0';
    ++repr_index;
  }
}

template <int N>
MP::Num<N>::Num(const std::string input_str)
    : repr(typename stl_type<N>::Type{0}), negative(false) {
  int repr_index = 0;
  // just use a reverse iterator
  for (auto it = input_str.rbegin(); it != input_str.rend(); it++) {
    if (*it < '0' || *it > '9') {
      if (*it == '-') {
        negative = true;
        break;
      }

      throw MP::Exception<N>(
          std::format("Number given isn't a number. Offending: {}", *it));
    }

    _at(repr_index) = *it - '0';
    repr_index++;
  }
}

// move constructor
template <int N>
MP::Num<N>::Num(Num<N>&& other) noexcept
    : repr(std::move(other.repr)), negative(other.negative) {}

// copy constructor
template <int N> MP::Num<N>::Num(const Num<N>& other) noexcept {
  negative = other.negative;
  repr = other.repr;
}

// copy assignment
template <int N> MP::Num<N>& MP::Num<N>::operator=(const Num<N>& other) {
  if (this == &other)
    return *this;

  MP::Num<N> temp(other);
  std::swap(repr, temp.repr);
  negative = temp.negative;

  return *this;
}

// move assignment
template <int N> MP::Num<N>& MP::Num<N>::operator=(Num<N>&& other) noexcept {
  MP::Num<N> temp(std::move(other));
  std::swap(repr, temp.repr);
  negative = temp.negative;
  return *this;
}

template <int N> MP::Num<N>::operator std::string() const {
  std::ostringstream s;
  if (negative) {
    s << "-";
  }
  bool filler = true;

  std::for_each(repr.rbegin(), repr.rend(), [&s, &filler](auto elem) {
    if (elem != 0 || !filler) {
      s << elem;
      // s << " ";
      filler = false;
    }
  });

  if (filler) {
    s << "0";
  }

  return std::string{s.str()};
}

template <int N>
std::ostream& operator<<(std::ostream& os, const MP::Num<N>& num) {
  os << static_cast<std::string>(num);
  return os;
}

template <int N> const short MP::Num<N>::last() const {
  return repr[repr.size - 1];
}

template <int N> MP::Num<N>& MP::Num<N>::operator<<(size_t shift_amount) {
  bool except = false;
  const size_t cont_size = repr.size();
  for (int i = cont_size - 1; i >= cont_size - shift_amount - 1; --i) {
    if (repr[i]) {
      except = true;
    }
  }

  for (int i = cont_size - 1; i >= static_cast<int>(shift_amount); --i) {
    repr[i] = repr[i - shift_amount];
  }

  for (size_t i = 0; i < shift_amount; ++i) {
    repr[i] = 0;
  }

  if (except) {
    throw MP::Exception<N>("Overflow [<<]", *this);
  }

  return *this;
}

template <int N>
template <int M>
MP::Num<N> MP::Num<N>::operator+=(const MP::Num<M>& rhs) {

  const bool lhs_neg = negative;
  const bool rhs_neg = rhs.is_negative();
  const auto lr_cmp = *this <=> rhs;

  if (!lhs_neg && rhs_neg) {
    return *this -= -rhs;
  }

  if (lhs_neg && !rhs_neg) {
    return (this->neg() -= rhs).neg();
  }

  if (lhs_neg && rhs_neg) {
    return (this->neg() += -rhs).neg();
  }

  short carry = 0;

  for (size_t i = 0; i < repr.size(); ++i) {
    const short lhs_num = i < repr.size() ? repr[i] : 0;
    const short rhs_num = i < rhs.size() ? rhs[i] : 0;
    const short temp = lhs_num + rhs_num + carry;
    repr[i] = temp % 10;
    carry = temp / 10;
  }

  if (carry) {
    throw MP::Exception<N>("Overflow [+=](carry)", *this);
  }

  return *this;
}

template <int N, int M>
MP::Num<template_max<N, M>()> operator+(const MP::Num<N>& lhs,
                                        const MP::Num<M>& rhs) {

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

  MP::Num<template_max<N, M>()> res;
  const size_t larger = lhs.size() > rhs.size() ? lhs.size() : rhs.size();

  short carry = 0;
  for (size_t i = 0; i < larger; ++i) {
    const short lhs_num = i < lhs.size() ? lhs[i] : 0;
    const short rhs_num = i < rhs.size() ? rhs[i] : 0;
    const short temp = lhs_num + rhs_num + carry;
    res[i] = temp % 10;
    carry = temp / 10;
  }

  if (carry) {
    throw MP::Exception<template_max<N, M>()>("Overflow [+](carry)", res);
  }

  return res;
}

template <int N>
template <int M>
MP::Num<N> MP::Num<N>::operator-=(const MP::Num<M>& rhs) {
  const bool lhs_neg = negative;
  const bool rhs_neg = rhs.is_negative();
  const auto lr_cmp = *this <=> rhs;

  if (lr_cmp == 0) {
    return MP::Num<N>{0};
  }

  if (!lhs_neg && rhs_neg) {
    return *this += -rhs;
  }

  if (lhs_neg && !rhs_neg) {
    return ((*this).neg() += rhs).neg();
  }

  if (lhs_neg && rhs_neg) {
    return ((*this).neg() -= -rhs).neg();
  }

  short carry = 0;
  for (size_t i = 0; i < repr.size(); ++i) {
    const short lhs_num = i < repr.size() ? repr[i] : 0;
    const short rhs_num = i < rhs.size() ? rhs[i] + carry : 0;
    short temp;

    if (lr_cmp < 0) {
      temp = rhs_num - lhs_num;
    } else {
      temp = lhs_num - rhs_num;
    }

    if (temp >= 0) {
      repr[i] = temp;
      carry = 0;
    } else {
      repr[i] = temp + 10;
      carry = 1;
    }
  }

  if (lr_cmp < 0) {
    negative = !negative;
  }

  if (carry) {
    throw MP::Exception<N>("Overflow [-=](carry)", *this);
  }

  return *this;
}

template <int N> MP::Num<N> operator-(const MP::Num<N>& num) {
  auto new_num = MP::Num<N>(num);
  new_num.neg();
  return new_num;
}

template <int N, int M>
MP::Num<template_max<N, M>()> operator-(const MP::Num<N>& lhs,
                                        const MP::Num<M>& rhs) {
  // subtraction implementation works only for positive numbers
  // so I created these rules to convert everything into addition/subtraction of
  // positive numbers
  const bool lhs_neg = lhs.is_negative();
  const bool rhs_neg = rhs.is_negative();
  const auto lr_cmp = lhs <=> rhs;

  if ((lhs <=> rhs) == 0) {
    return MP::Num<template_max<N, M>()>{0};
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

  MP::Num<template_max<N, M>()> res;
  const size_t larger = lhs.size() > rhs.size() ? lhs.size() : rhs.size();

  short carry = 0;
  for (size_t i = 0; i < larger; ++i) {
    const short lhs_num = i < lhs.size() ? lhs[i] : 0;
    const short rhs_num = i < rhs.size() ? rhs[i] + carry : 0;
    if (lhs_num - rhs_num >= 0) {
      res[i] = lhs_num - rhs_num;
      carry = 0;
    } else {
      res[i] = (lhs_num + 10) - rhs_num;
      carry = 1;
    }
  }

  if (carry) {
    throw MP::Exception<template_max<N, M>()>("Oveflow [-](carry)", res);
  }

  return res;
}

template <int N>
MP::Num<N> operator*(const MP::Num<N>& num, const MP::trivial auto mul) {
  MP::Num<N> res;

  short carry = 0;
  for (size_t i = 0; i < num.size(); ++i) {
    const short temp = num[i] * mul + carry;
    res[i] = temp % 10;
    carry = temp / 10;
  }

  if (carry) {
    throw MP::Exception<N>("Oveflow [*](carry)", res);
  }

  return res;
}

template <int N, int M>
MP::Num<template_max<N, M>()> operator*(const MP::Num<N>& lhs,
                                        const MP::Num<M>& rhs) {
  MP::Num<template_max<N, M>()> res_num{};
  const size_t larger = lhs.size() > rhs.size() ? lhs.size() : rhs.size();
  for (size_t i = 0; i < larger; ++i) {
    auto temp_num = lhs * rhs[i];
    if ((temp_num <=> MP::Num<1>{0}) != 0) {
      res_num += temp_num << i;
    }
  }

  if (lhs.is_negative() && !rhs.is_negative() ||
      !lhs.is_negative() && rhs.is_negative()) {
    res_num.neg();
  }

  return res_num;
}

template <int N>
template <int M>
MP::Num<N> MP::Num<N>::operator*=(const MP::Num<M>& rhs) {
  MP::Num<N> res_num{};
  for (size_t i = 0; i < repr.size(); ++i) {
    auto temp_num = *this * rhs[i];
    if ((temp_num <=> MP::Num<1>{0}) != 0) {
      res_num += temp_num << i;
    }
  }

  if (negative && !rhs.is_negative() || !negative && rhs.is_negative()) {
    res_num.neg();
  }

  repr = std::move(res_num.repr);
  negative = res_num.negative;

  return *this;
}

template <int N, int M>
MP::Num<template_max<N, M>()> operator/(const MP::Num<N>& lhs,
                                        const MP::Num<M>& rhs) {
  constexpr const size_t resulting_size = template_max<N, M>();

  MP::Num<resulting_size> res{0};
  auto temp = lhs.abs();
  auto sub_abs = rhs.abs();
  while ((temp <=> rhs) > 0) {
    res += MP::Num<resulting_size>{1};
    temp -= sub_abs;
  }

  if (lhs.is_negative() && !rhs.is_negative() ||
      !lhs.is_negative() && rhs.is_negative()) {
    res.neg();
  }

  return res;
}

template <int N>
template <int M>
MP::Num<N> MP::Num<N>::operator/=(const MP::Num<M>& rhs) {
  MP::Num<N> res{0};
  auto temp = MP::Num<N>{*this}.abs();
  auto sub_abs = rhs.abs();

  while ((temp <=> rhs) > 0) {
    res += MP::Num<N>{1};
    temp -= sub_abs;
  }

  if (negative && !rhs.is_negative() || !negative && rhs.is_negative()) {
    res.neg();
  }

  repr = std::move(res.repr);
  negative = res.negative;

  return *this;
}

template <int N, int M>
auto operator<=>(const MP::Num<N>& lhs, const MP::Num<M>& rhs) {
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

  for (size_t i = lhs.size(); i--;) {
    if (lhs[i]) {
      lhs_index = i;
      break;
    }
  }

  for (size_t i = rhs.size(); i--;) {
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

template <int N> MP::Num<N> MP::Num<N>::factorial() const {
  if (negative) {
    throw MP::Exception<N>("Factorial cannot be performed on negative numbers");
  }

  MP::Num<N> res{1};

  for (MP::Num<N> i{1}; (i <=> *this) <= 0; i += MP::Num<N>{1}) {
    res *= i;
  }

  return res;
}
