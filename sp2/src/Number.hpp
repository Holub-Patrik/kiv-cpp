#include "NumberDef.hpp"
#include "NumberEx.hpp"

#include <algorithm>
#include <cstdint>
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
    throw MP::Exception<N>("Array out of bounds access", *this);
  }

  return repr[index];
}

template <int N> MP::repr_type& MP::Num<N>::_at(size_t index) {
  if (index >= repr.size()) {
    throw MP::Exception<N>("Array out of bounds access", *this);
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
const MP::repr_type MP::Num<N>::_safe_at(size_t index) const noexcept {
  if (index >= repr.size()) {
    MP::repr_type ret_val = 0;

    if (is_negative()) {
      ret_val = ~ret_val;
    }

    return ret_val;
  }

  return repr[index];
}

template <int N> MP::repr_type& MP::Num<N>::_safe_at(size_t index) {
  if (index >= repr.size()) {
    _absorb = 0;
    if (is_negative()) {
      _absorb = ~_absorb; // just negate as 2's complement of 0 is 0 and not the
                          // max value I am looking for
    }
    return _absorb;
  }

  return repr[index];
}

// inline due to cland warning and auto fix
template <>
inline MP::repr_type& MP::Num<MP::Unlimited>::_safe_at(size_t index) {
  if (index >= repr.size()) {
    repr.resize(index / 10 + 10, 0);
  }

  return repr[index];
}

template <int N>
MP::Num<N>::Num(const std::int32_t num) : repr(typename stl_type<N>::Type{0}) {
  bool negative = num < 0;

  if (negative) {
    repr[0] = -num;
    neg();
  } else {
    repr[0] = num;
  }
}

template <int N> MP::Num<N>& MP::Num<N>::neg() noexcept {
  for (auto it = repr.begin(); it != repr.end(); it++) {
    *it = ~(*it); // flip all the parts
  }

  *this += MP::Num<N>{1};
  return *this;
}

template <int N>
MP::Num<N>::Num(const char* input_str) : repr(typename stl_type<N>::Type{0}) {
  // first find \0 character, from there go in reverse
  int length = 0;
  bool is_numeric = true;
  char wrong_char = 0;
  bool negative = false;

  for (int i = 0; char a = input_str[i]; i++) {
    // should be checked using a locale, but I know this won't be run somewhere
    // weird
    if (a < '0' || a > '9') {
      if (a == '-') {
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

  int repr_index = 0;
  for (int i = length; i--;) {
    if (input_str[i] == '-') {
      negative = true;
      break;
    }
    _at(repr_index) = input_str[i] - '0';
    ++repr_index;
  }
}

template <int N>
MP::Num<N>::Num(const std::string input_str)
    : repr(typename stl_type<N>::Type{0}) {
  bool negative = false;
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
MP::Num<N>::Num(Num<N>&& other) noexcept : repr(std::move(other.repr)) {}

// copy constructor
template <int N> MP::Num<N>::Num(const Num<N>& other) noexcept {
  repr = other.repr;
}

// copy assignment
template <int N> MP::Num<N>& MP::Num<N>::operator=(const Num<N>& other) {
  if (this == &other)
    return *this;

  MP::Num<N> temp(other);
  std::swap(repr, temp.repr);

  return *this;
}

// move assignment
template <int N> MP::Num<N>& MP::Num<N>::operator=(Num<N>&& other) noexcept {
  MP::Num<N> temp(std::move(other));
  std::swap(repr, temp.repr);
  return *this;
}

template <int N> MP::Num<N>::operator std::string() const {
  std::ostringstream s;
  auto out_cp{*this};
  if (out_cp.is_negative()) {
    s << "- ";
    out_cp.neg();
  }

  if ((out_cp <=> MP::Num<N>{0}) == 0) {
    s << 0;
  }

  auto ten = MP::Num<N>{10};
  std::vector<int> out_buf{};
  while ((out_cp <=> MP::Num<N>{0}) > 0) {
    auto char_num = mod_10<N>(out_cp);
    out_cp = out_cp / ten;
    out_buf.push_back(char_num.get_repr()[0]);
  }

  std::for_each(out_buf.rbegin(), out_buf.rend(), [&s](auto elem) {
    s << elem;
    s << " ";
  });

  // for (auto it = out_cp.repr.rbegin(); it != out_cp.repr.rend(); it++) {
  //   s << *it;
  //   s << " ";
  // }

  return std::string{s.str()};
}

template <int N>
std::ostream& operator<<(std::ostream& os, const MP::Num<N>& num) {
  os << static_cast<std::string>(num);
  return os;
}

template <int N> const short MP::Num<N>::last() const {
  return repr[repr.size() - 1];
}

/*
 * Byt shift by shift_amount bits
 */
template <int N> MP::Num<N>& MP::Num<N>::operator<<(const size_t shift_amount) {
  bool except = false;

  // transform shift amount to vals * 32 + bits
  const int vals = shift_amount / 32; // amount of 32 bit values during shift
  const int bits = shift_amount % 32; // amount of bits during shift

  auto rev_it = repr.rbegin();
  for (int i = 0; i < vals; ++i) {
    const auto test_val = *rev_it;
    if (test_val != 0) {
      except = true;
    }
    ++rev_it;
  }

  const auto test_val = *rev_it;
  auto check_bits_val = (*rev_it) >> (32 - bits);
  if (check_bits_val) {
    except = true;
  }

  for (int i = repr.size() - 1; i > vals; --i) {
    repr[i] = repr[i - vals];
  }

  for (int i = repr.size(); i > 1; --i) {
    std::uint64_t temp = static_cast<std::uint64_t>(repr[i]);
    std::uint64_t temp_next = static_cast<std::uint64_t>(repr[i - 1]);

    temp <<= bits;
    temp_next >>= 32 - bits;
    temp += temp_next;

    repr[i] = static_cast<std::uint32_t>(temp);
  }
  // last operation not done by the loop;
  repr[0] <<= bits;

  if (except) {
    throw MP::Exception<N>("Overflow [<<]", *this);
  }

  return *this;
}

template <>
inline MP::Num<MP::Unlimited>&
MP::Num<MP::Unlimited>::operator<<(const size_t shift_amount) {
  // fix here
  for (size_t i = repr.size() + shift_amount; i > shift_amount; --i) {
    _at(i) = _at(i - shift_amount);
  }

  for (int i = 0; i < shift_amount; ++i) {
    _at(i) = 0;
  }

  return *this;
}

template <int N> MP::Num<N> mod_10(const MP::Num<N>& lhs) {
  auto c = lhs / MP::Num<N>{10};
  return lhs - (c * MP::Num<N>{10});
}

template <int N>
template <int M>
MP::Num<N> MP::Num<N>::operator+=(const MP::Num<M>& rhs) {
  // bug here with unlimited precision

  std::uint64_t carry = 0;
  for (size_t i = 0; i < repr.size(); ++i) {
    const std::uint64_t lhs_num = static_cast<std::uint64_t>(_safe_at(i));
    const std::uint64_t rhs_num = static_cast<std::uint64_t>(rhs._safe_at(i));
    const std::uint64_t temp = lhs_num + rhs_num + carry;
    carry = temp >> 32;
    _at(i) = static_cast<std::uint32_t>(temp - (carry << 32));
  }

  // overflow exception has to be done better
  // if (carry) {
  //   throw MP::Exception<N>("Overflow [+=](carry)", *this);
  // }

  return *this;
}

template <int N, int M>
MP::Num<template_max<N, M>()> operator+(const MP::Num<N>& lhs,
                                        const MP::Num<M>& rhs) {

  MP::Num<template_max<N, M>()> res;
  const size_t larger = lhs.size() > rhs.size() ? lhs.size() : rhs.size();

  std::uint64_t carry = 0;
  for (size_t i = 0; i < larger; ++i) {
    const std::uint64_t lhs_num = static_cast<uint64_t>(lhs._safe_at(i));
    const std::uint64_t rhs_num = static_cast<uint64_t>(rhs._safe_at(i));
    const std::uint64_t temp = lhs_num + rhs_num + carry;
    carry = temp >> 32; // get the carry
    const std::uint64_t test = temp - (carry << 32);
    res[i] = static_cast<uint32_t>(temp - (carry << 32)); // subtract the carry
  }

  // overflow exception has to be done better
  // if (carry) {
  //   throw MP::Exception<template_max<N, M>()>("Overflow [+](carry)", res);
  // }

  return res;
}

template <int N>
template <int M>
MP::Num<N> MP::Num<N>::operator-=(const MP::Num<M>& rhs) {
  *this += (-rhs);
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
  return lhs + (-rhs);
}

template <int N, int M>
MP::Num<template_max<N, M>()> operator*(const MP::Num<N>& lhs,
                                        const MP::Num<M>& rhs) {
  MP::Num<template_max<N, M>()> res{0};
  const size_t larger = lhs.size() > rhs.size() ? lhs.size() : rhs.size();
  // take absolute values, since I do not know what will happen if I don't
  // and I do not want to mess with that logic
  auto lhs_abs = lhs.abs();
  auto rhs_abs = rhs.abs();

  // n^2 algorithm, since I am dumb. To be honest, c++ templates and
  // thinking about this logic was a bit too much for me
  std::uint64_t carry;
  for (size_t i = 0; i < lhs_abs.size(); ++i) {
    carry = 0;
    const std::uint64_t lhs_num =
        static_cast<std::uint64_t>(lhs_abs._safe_at(i));
    MP::Num<M> temp_num{0};

    for (size_t j = 0; j < rhs_abs.size(); ++j) {
      const std::uint64_t rhs_num =
          static_cast<std::uint64_t>(rhs_abs._safe_at(j));
      const std::uint64_t temp = lhs_num * rhs_num + carry;
      carry = temp >> 32;
      temp_num[j] = static_cast<std::uint32_t>(temp - (carry << 32));
    }
    const auto test = temp_num << (i * 32);
    res += (temp_num << (i * 32));
  }

  if (lhs.is_negative() && !rhs.is_negative() ||
      rhs.is_negative() && !lhs.is_negative()) {
    res.neg();
  }

  // overflow exception has to be done better
  if (carry) {
    throw MP::Exception<template_max<N, M>()>("Overflow [*](carry)", res);
  }

  return res;
}

template <int N>
template <int M>
MP::Num<N> MP::Num<N>::operator*=(const MP::Num<M>& rhs) {
  MP::Num<N> res{0};
  // take absolute values, since I do not know what will happen if I don't
  // and I do not want to mess with that logic
  auto lhs_abs = abs();
  auto rhs_abs = rhs.abs();

  // n^2 algorithm, since I am dumb. To be honest, c++ templates and
  // thinking about this logic was a bit too much for me
  std::uint64_t carry;
  for (size_t i = 0; i < lhs_abs.size(); ++i) {
    carry = 0;
    const std::uint64_t lhs_num =
        static_cast<std::uint64_t>(lhs_abs._safe_at(i));
    MP::Num<M> temp_num{0};

    for (size_t j = 0; j < rhs_abs.size(); ++j) {
      const std::uint64_t rhs_num =
          static_cast<std::uint64_t>(rhs_abs._safe_at(j));
      const std::uint64_t temp = lhs_num * rhs_num + carry;
      carry = temp >> 32;
      temp_num[j] = static_cast<std::uint32_t>(temp - (carry << 32));
    }
    const auto test = temp_num << (i * 32);
    res += (temp_num << (i * 32));
  }

  if (is_negative() && !rhs.is_negative() ||
      rhs.is_negative() && !is_negative()) {
    res.neg();
  }

  // overflow exception has to be done better
  if (carry) {
    throw MP::Exception<template_max<N, M>()>("Overflow [*](carry)", res);
  }

  *this = std::move(res);
  return *this;
}

template <int N, int M>
MP::Num<template_max<N, M>()> operator/(const MP::Num<N>& lhs,
                                        const MP::Num<M>& rhs) {
  constexpr const size_t resulting_size = template_max<N, M>();

  MP::Num<resulting_size> res{0};
  auto temp = lhs.abs();
  auto sub_abs = rhs.abs();
  while ((temp <=> rhs) >= 0) {
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

  repr = std::move(res.repr);

  if (is_negative() && !rhs.is_negative() ||
      !is_negative() && rhs.is_negative()) {
    res.neg();
  }

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
  if (is_negative()) {
    throw MP::Exception<N>("Factorial cannot be performed on negative numbers",
                           *this);
  }

  MP::Num<N> res{1};

  for (MP::Num<N> i{1}; (i <=> *this) <= 0; i += MP::Num<N>{1}) {
    res *= i;
    std::cout << res << std::endl;
  }

  return res;
}
