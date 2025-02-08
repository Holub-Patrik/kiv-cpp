#include "NumberDef.hpp"
#include "NumberEx.hpp"

#include <algorithm>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <format>
#include <iostream>
#include <limits>
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
template <>
inline MP::repr_type& MP::Num<MP::Unlimited>::_at(const size_t index) {
  if (index >= repr.size()) {
    repr.resize(index + 10, 0);
  }

  return repr[index];
}

template <int N>
const MP::repr_type MP::Num<N>::_safe_at(size_t index) const noexcept {
  if (index >= repr.size()) {
    MP::repr_type ret_val = 0;

    if (is_negative()) {
      ret_val = std::numeric_limits<MP::repr_type>::max();
    }

    return ret_val;
  }

  return repr[index];
}

template <int N> MP::repr_type& MP::Num<N>::_safe_at(size_t index) {
  if (index >= repr.size()) {
    _absorb = 0;
    if (is_negative()) {
      _absorb = std::numeric_limits<
          MP::repr_type>::max(); // just negate as 2's complement of 0 is 0 and
                                 // not the max value I am looking for
    }
    return _absorb;
  }

  return repr[index];
}

// inline due to cland warning and auto fix
template <>
inline MP::repr_type& MP::Num<MP::Unlimited>::_safe_at(const size_t index) {
  if (index >= repr.size()) {
    // std::cout << "new_size: " << index + 10 << std::endl;
    repr.resize(index + 10, 0);
  }

  return repr[index];
}

template <int N>
MP::Num<N>::Num(const std::int64_t num) : repr(typename stl_type<N>::Type{0}) {
  bool negative = num < 0;
  const std::int64_t abs = num >= 0 ? num : -num;

  _at(0) = static_cast<std::uint32_t>(abs);
  _at(1) = static_cast<std::uint32_t>(abs >> 32);

  if (negative) {
    neg();
  }
}

template <int N> MP::Num<N>& MP::Num<N>::neg() noexcept {
  for (auto it = repr.begin(); it != repr.end(); it++) {
    *it = ~(*it); // flip all the parts
  }

  std::uint64_t carry = 1;
  for (auto& elem : repr) {
    const std::uint64_t elem_cp = static_cast<std::uint64_t>(elem);
    const std::uint64_t temp = elem_cp + carry;
    elem = static_cast<std::uint32_t>(temp);
    carry = temp >> 32;
  }

  return *this;
}

template <int N>
MP::Num<N>::Num(const char* input_str) : Num(std::string{input_str}) {}

template <int N>
MP::Num<N>::Num(const std::string input_str)
    : repr(typename stl_type<N>::Type{0}) {
  bool negative = false;
  bool minus_sign_allowed = true;
  bool first_digit = true;
  for (auto it = input_str.begin(); it != input_str.end(); it++) {
    if (*it < '0' || *it > '9') {
      if (*it == '-' && minus_sign_allowed) {
        negative = true;
        minus_sign_allowed = false;
        continue;
      } else {
        throw MP::Exception<N>(
            std::format("Number given isn't a number. Offending: {}", *it));
      }
    }

    if (first_digit) {
      *this = MP::Num<N>{*it - '0'};
      minus_sign_allowed = false;
      first_digit = false;
      continue;
    }

    *this *= MP::Num<N>{10};
    *this += MP::Num<N>{*it - '0'};
  }

  if (negative) {
    neg();
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

  if ((*this <=> MP::Num<N>{0L}) == 0) {
    s << 0;
    return std::string{s.str()};
  }

  auto out_cp{*this};
  if (out_cp.is_negative()) {
    s << "-";
    out_cp.neg();
  }

  if ((out_cp <=> MP::Num<N>{1}) == 0) {
    s << "1";
    return std::string{s.str()};
  }

  MP::Num<N> helper{1};
  std::vector<MP::Num<N>> lookup{};
  int dbg_out = 0;
  while ((helper <=> out_cp) < 0) {
    // std::cout << dbg_out++ << std::endl;
    lookup.push_back(helper);
    helper *= MP::Num<N>{10};
  }

  int temp = 0;
  for (int i = lookup.size() - 1; i >= 0; i--) {
    while ((out_cp <=> lookup[i]) >= 0) {
      out_cp -= lookup[i];
      temp++;
    }
    s << temp;
    temp = 0;
  }

  return std::string{s.str()};
}

template <int N>
std::ostream& operator<<(std::ostream& os, const MP::Num<N>& num) {
  os << static_cast<std::string>(num);
  return os;
}

template <int N> const bool MP::Num<N>::is_negative() const noexcept {
  return (last() >> 31) & 1;
}

template <int N> const MP::repr_type MP::Num<N>::last() const {
  return *repr.rbegin();
}

/*
 * Byt shift by shift_amount bits
 */
template <int N> MP::Num<N>& MP::Num<N>::operator<<(const size_t shift_amount) {
  if (shift_amount == 0) {
    return *this;
  }
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
  if (shift_amount == 0) {
    return *this;
  }
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
MP::Num<N>& MP::Num<N>::operator+=(const MP::Num<M>& rhs) {
  const auto lhs_abs = abs();
  const auto rhs_abs = rhs.abs();

  const bool lhs_pos = !is_negative();
  const bool rhs_pos = !rhs.is_negative();

  const auto cmp = lhs_abs <=> rhs_abs;
  const auto uint32_max = std::numeric_limits<std::uint32_t>::max();

  if (cmp == 0) {
    if (lhs_pos && !rhs_pos) {
      *this = std::move(MP::Num<N>{0L});
      return *this;
    }
    if (!lhs_pos && rhs_pos) {
      *this = std::move(MP::Num<N>{0L});
      return *this;
    }
  }

  std::uint32_t carry_comprehension = 0;
  if (lhs_pos && !rhs_pos) {
    if (cmp > 0) {
      carry_comprehension = 0;
    }
    if (cmp < 0) {
      carry_comprehension = uint32_max;
    }
  }

  if (!lhs_pos && rhs_pos) {
    if (cmp > 0) {
      carry_comprehension = uint32_max;
    }
    if (cmp < 0) {
      carry_comprehension = 0;
    }
  }

  int loop_end = size();
  if (N == MP::Unlimited) {
    loop_end = rhs.size() > size() ? rhs.size() : size();
  }
  std::uint64_t carry = 0;
  // i outside used for assigning carry
  int i = 0;
  for (; i < loop_end; i++) {
    const std::uint64_t lhs_num = static_cast<std::uint64_t>(_safe_at(i));
    const std::uint64_t rhs_num = static_cast<std::uint64_t>(rhs._safe_at(i));
    const std::uint64_t temp = lhs_num + rhs_num + carry;
    carry = static_cast<std::uint32_t>(temp >> 32);

    _at(i) = static_cast<std::uint32_t>(temp);
  }

  if (carry > 0) {
    if (lhs_pos && rhs_pos) {
      _safe_at(i) = carry;
    } else if (!lhs_pos && !rhs_pos) {
      _safe_at(i) = uint32_max;
    } else {
      _safe_at(i) = carry_comprehension;
    }
  }

  if (is_negative() && lhs_pos && rhs_pos) {
    throw MP::Exception<N>("Overflow (++ => -)[+=]", *this);
  }

  if (!is_negative() && !lhs_pos && !rhs_pos) {
    throw MP::Exception<N>("Overflow (-- => +)[+=]", *this);
  }

  return *this;
}

template <int N, int M>
MP::Num<template_max<N, M>()> operator+(const MP::Num<N>& lhs,
                                        const MP::Num<M>& rhs) {

  const auto lhs_abs = lhs.abs();
  const auto rhs_abs = rhs.abs();

  const bool lhs_pos = !lhs.is_negative();
  const bool rhs_pos = !rhs.is_negative();

  const auto cmp = lhs_abs <=> rhs_abs;
  const auto uint32_max = std::numeric_limits<std::uint32_t>::max();

  if (cmp == 0) {
    if (lhs_pos && !rhs_pos) {
      return MP::Num<template_max<N, M>()>{0L};
    }
    if (!lhs_pos && rhs_pos) {
      return MP::Num<template_max<N, M>()>{0L};
    }
  }

  std::uint32_t carry_comprehension = 0;
  if (lhs_pos && rhs_pos) {
    carry_comprehension = 0;
  }

  if (lhs_pos && !rhs_pos) {
    if (cmp > 0) {
      carry_comprehension = 0;
    }
    if (cmp < 0) {
      carry_comprehension = uint32_max;
    }
  }

  if (!lhs_pos && rhs_pos) {
    if (cmp > 0) {
      carry_comprehension = uint32_max;
    }
    if (cmp < 0) {
      carry_comprehension = 0;
    }
  }

  if (!lhs_pos && !rhs_pos) {
    carry_comprehension = uint32_max;
  }

  MP::Num<template_max<N, M>()> res;
  const size_t larger = lhs.size() > rhs.size() ? lhs.size() : rhs.size();

  std::uint64_t carry = 0;
  // outside for later assigning carry
  size_t i = 0;
  for (; i < larger; ++i) {
    const std::uint64_t lhs_num = static_cast<uint64_t>(lhs._safe_at(i));
    const std::uint64_t rhs_num = static_cast<uint64_t>(rhs._safe_at(i));
    const std::uint64_t temp = lhs_num + rhs_num + carry;

    carry = temp >> 32;                   // get the carry
    res[i] = static_cast<uint32_t>(temp); // subtract the carry
  }

  if (carry > 0) {
    if (lhs_pos && rhs_pos) {
      res._safe_at(i) = carry;
    } else if (!lhs_pos && !rhs_pos) {
      res._safe_at(i) = uint32_max;
    } else {
      res._safe_at(i) = carry_comprehension;
    }
    i++;
  }

  for (; i < res.size(); i++) {
    res[i] = carry_comprehension;
  }

  if (res.is_negative() && lhs_pos && rhs_pos) {
    throw MP::Exception<template_max<N, M>()>("Overflow (++ => -)[+=]", res);
  }

  if (!res.is_negative() && !lhs_pos && !rhs_pos) {
    throw MP::Exception<template_max<N, M>()>("Overflow (-- => +)[+=]", res);
  }

  return res;
}

template <int N>
template <int M>
MP::Num<N>& MP::Num<N>::operator-=(const MP::Num<M>& rhs) {
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
  MP::Num<template_max<N, M>()> res{0L};
  // take absolute values, since I do not know what will happen if I don't
  // and I do not want to mess with that logic
  const bool lhs_neg = lhs.is_negative();
  const bool rhs_neg = rhs.is_negative();
  const auto lhs_abs = lhs.abs();
  const auto rhs_abs = rhs.abs();

  // n^2 algorithm, since I am dumb. To be honest, c++ templates and
  // thinking about this logic was a bit too much for me
  std::uint64_t carry;
  for (size_t i = 0; i < lhs_abs.size(); ++i) {
    res += ((lhs_abs * rhs_abs[i]) << i * 32);
  }

  if ((lhs_neg && !rhs_neg) || (!lhs_neg && rhs_neg)) {
    res.neg();
  }

  return res;
}

template <int N>
template <int M>
MP::Num<N>& MP::Num<N>::operator*=(const MP::Num<M>& rhs) {
  MP::Num<N> res{0L};
  // take absolute values, since I do not know what will happen if I don't
  // and I do not want to mess with that logic
  const bool lhs_neg = is_negative();
  const bool rhs_neg = rhs.is_negative();
  const auto lhs_abs = abs();
  const auto rhs_abs = rhs.abs();

  // n^2 algorithm, since I am dumb. To be honest, c++ templates and
  // thinking about this logic was a bit too much for me
  std::uint64_t carry;
  for (size_t i = 0; i < rhs_abs.size(); ++i) {
    res += ((lhs_abs * rhs_abs[i]) << i * 32);
  }

  if ((lhs_neg && !rhs_neg) || (!lhs_neg && rhs_neg)) {
    res.neg();
  }

  *this = std::move(res);
  return *this;
}

template <int N>
MP::Num<N> operator*(const MP::Num<N>& lhs, std::uint32_t rhs) {
  MP::Num<N> res{0L};
  std::uint64_t carry = 0;
  const auto lhs_abs = lhs.abs();
  int i = 0;
  for (; i < lhs_abs.size(); i++) {
    const std::uint64_t lhs_num = static_cast<std::uint64_t>(lhs_abs[i]);
    const std::uint64_t rhs_num = static_cast<std::uint64_t>(rhs);
    const std::uint64_t temp = lhs_num * rhs_num + carry;
    res[i] = static_cast<std::uint32_t>(temp);
    carry = temp >> 32;
  }

  if (carry > 0) {
    try {
      res._at(i) = carry;
    } catch (MP::Exception<N> ex) {
      throw MP::Exception<N>("Overflow [*k](carry)", res);
    }
  }

  if ((lhs.is_negative() && rhs > 0) || (!lhs.is_negative() && rhs < 0)) {
    res.neg();
  }

  return res;
}

template <int N, int M>
MP::Num<template_max<N, M>()> operator/(const MP::Num<N>& lhs,
                                        const MP::Num<M>& rhs) {
  constexpr const size_t resulting_size = template_max<N, M>();
  const bool lhs_neg = lhs.is_negative();
  const bool rhs_neg = rhs.is_negative();

  MP::Num<resulting_size> res{0L};
  auto temp = lhs.abs();
  auto sub_abs = rhs.abs();
  while ((temp <=> rhs) >= 0) {
    res += MP::Num<resulting_size>{1};
    temp -= sub_abs;
  }

  if ((lhs_neg && !rhs_neg) || (!lhs_neg && rhs_neg)) {
    res.neg();
  }

  return res;
}

template <int N>
template <int M>
MP::Num<N>& MP::Num<N>::operator/=(const MP::Num<M>& rhs) {
  MP::Num<N> res{0L};
  const bool lhs_neg = is_negative();
  const bool rhs_neg = rhs.is_negative();
  auto temp = MP::Num<N>{*this}.abs();
  auto sub_abs = rhs.abs();

  while ((temp <=> rhs) > 0) {
    res += MP::Num<N>{1};
    temp -= sub_abs;
  }

  if ((lhs_neg && !rhs_neg) || (!lhs_neg && rhs_neg)) {
    res.neg();
  }

  repr = std::move(res.repr);

  return *this;
}

template <int N>
auto operator<=>(const MP::Num<N> lhs, const std::int64_t rhs) {
  if (lhs.is_negative() && rhs > 0) {
    return std::strong_ordering::less;
  } else if (!lhs.is_negative() && rhs < 0) {
    return std::strong_ordering::greater;
  }

  const auto lhs_abs = lhs.abs();
  const auto rhs_abs = static_cast<std::uint64_t>(rhs > 0 ? rhs : -rhs);

  // safe since all numbers will have at least 4 bytes
  if (lhs_abs[3]) {
    return std::strong_ordering::greater;
  }

  std::uint64_t lhs_cmp_num;
  lhs_cmp_num = static_cast<std::uint64_t>(lhs_abs[0]);
  lhs_cmp_num += static_cast<std::uint64_t>(lhs_abs[1]) << 32;

  return lhs_cmp_num <=> rhs_abs;
}

template <int N>
auto operator<=>(const std::int64_t lhs, const MP::Num<N> rhs) {
  return 0 <=> (lhs <=> rhs);
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
    // std::cout << i << " : " << res << std::endl;
    // std::cout << res << std::endl;
  }

  return res;
}
