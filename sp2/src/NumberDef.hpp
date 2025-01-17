#pragma once
#include <array>
#include <concepts>
#include <string>
#include <vector>

template <int N, int M> consteval int template_max();

namespace MP {
typedef unsigned int repr_type;

template <typename T>
concept trivial = std::integral<T> && requires(T t) { t >= 0 && t < 10; };

template <typename T>
concept pos_int = std::integral<T> && requires(T t) { t > 0; };

constexpr int Unlimited = -1;

template <int N> struct stl_type {
  using Type = std::array<repr_type, N>;
};

template <> struct stl_type<Unlimited> {
  using Type = std::vector<repr_type>;
};

template <int N> class Num final {
private:
  stl_type<N>::Type repr;
  bool negative;

public:
  Num() : repr(typename stl_type<N>::Type{0}), negative(false) {}
  ~Num() {}

  Num(long);
  Num(stl_type<N>::Type&&);
  Num(Num<N>&&) noexcept;
  Num(const Num<N>&) noexcept;

  Num& operator=(const Num<N>&);
  Num& operator=(Num<N>&&) noexcept;

  const stl_type<N>::Type& get_repr() const noexcept { return repr; };
  std::size_t size() const noexcept { return repr.size(); };
  const bool is_negative() const noexcept { return negative; }
  Num<N>& neg() noexcept {
    negative = !negative;
    return *this;
  }

  Num<N> abs() const {
    auto res = Num<N>{*this};
    if (res.is_negative()) {
      res.neg();
    }
    return res;
  }

  Num<N> factorial() const;

  template <int M> Num<N> operator+=(const Num<M>& rhs);

  template <int M> Num<N> operator-=(const Num<M>& rhs);

  template <int M> Num<N> operator/=(const Num<M>& rhs);

  template <int M> Num<N> operator*=(const Num<M>& rhs);

  const repr_type operator[](size_t index) const;
  repr_type& operator[](size_t index);
  const repr_type _at(size_t index) const;
  repr_type& _at(size_t index);
  Num<N>& operator<<(size_t);

  operator std::string() const;
  const short last() const;
};

} // namespace MP

template <int N, int M>
auto operator<=>(const MP::Num<N>& lhs, const MP::Num<M>& rhs);

template <int N, int M>
MP::Num<template_max<N, M>()> operator+(const MP::Num<N>& lhs,
                                        const MP::Num<M>& rhs);

template <int N> MP::Num<N> operator-(const MP::Num<N>& num);
template <int N, int M>
MP::Num<template_max<N, M>()> operator-(const MP::Num<N>& lhs,
                                        const MP::Num<M>& rhs);

template <int N>
MP::Num<N> operator*(const MP::Num<N>& num, const MP::trivial auto mul);

template <int N, int M>
MP::Num<template_max<N, M>()> operator*(const MP::Num<N>& lhs,
                                        const MP::Num<M>& rhs);

template <int N, int M>
MP::Num<template_max<N, M>()> operator/(const MP::Num<N>& lhs,
                                        const MP::Num<M>& rhs);
