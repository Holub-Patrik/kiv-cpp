#pragma once
#include <array>
#include <concepts>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

// template max for return statements where addition of 2 template numbers
// have to be forced into the larger type
template <int N, int M> consteval int template_max();

namespace MP {
// typedef for internal representation type
typedef std::uint32_t repr_type;

// used for simple multiplication aka N * [0-9]
template <typename T>
concept trivial = std::integral<T> && requires(T t) { t >= 0 && t < 10; };

// -1 used for unlimited specialization
constexpr int Unlimited = -1;

// Thank you very much for this hint
// Without it I most likely would have gone insane
// Although when I tried to do it with an empty struct and then
// do 2 specializations, it wouldn't compile. Don't know if I did
// something wrong, but this works.

// base class for limited
template <int N> struct stl_type {
  using Type = std::array<repr_type, N>;
};

// in case of -1 => std::vector implementation
template <> struct stl_type<Unlimited> {
  using Type = std::vector<repr_type>;
};

template <int N> class Num final {
private:
  stl_type<N>::Type repr;
  repr_type _absorb;

public:
  Num()
      : repr(typename stl_type<N>::Type{0}),
        _absorb(std::numeric_limits<repr_type>::max()) {}
  ~Num() {}

  // QOL constructors
  Num(const std::int64_t);
  Num(stl_type<N>::Type&&);
  explicit Num(const char*);
  Num(const std::string);
  // copy conversion constructor from another number
  template <int M> Num(const Num<M>&);
  // copy conversion assignment operator from another number
  template <int M> Num& operator=(const Num<M>&);

  // move constructor
  Num(Num<N>&&) noexcept;
  // copy constructor
  Num(const Num<N>&) noexcept;

  // move assignment
  Num& operator=(Num<N>&&) noexcept;
  // copy assignment
  Num& operator=(const Num<N>&);

  // essentially getters for neccessary parts of the class
  const stl_type<N>::Type& get_repr() const noexcept { return repr; };
  std::size_t size() const noexcept { return repr.size(); };

  const bool is_negative() const noexcept;

  // change the number from negative to positive
  Num<N>& neg() noexcept;

  // return absolute number aka set negative to false
  Num<N> abs() const {
    auto res = Num<N>{*this};
    if (res.is_negative()) {
      res.neg();
    }
    return res;
  }

  // factorial
  Num<N> factorial() const;

  // operation assignment operators declared internally
  template <int M> Num<N>& operator+=(const Num<M>& rhs);
  template <int M> Num<N>& operator-=(const Num<M>& rhs);
  template <int M> Num<N>& operator/=(const Num<M>& rhs);
  template <int M> Num<N>& operator*=(const Num<M>& rhs);

  // access primitives to be used internaly
  const repr_type _at(size_t index) const;
  repr_type& _at(size_t index);
  const repr_type _safe_at(size_t index) const noexcept;
  // this wouldn't make sense in N != Unlimited so it is left unimplemented
  MP::repr_type& _safe_at(size_t index);
  // QOL operator overload
  const repr_type operator[](size_t index) const;
  repr_type& operator[](size_t index);
  Num<N>& operator<<(const size_t);
  // string printing overloads so I can easily print
  operator std::string() const;

  // quick access to last element of the internal representation
  const MP::repr_type last() const;
};

} // namespace MP

template <int N, int M>
auto operator<=>(const MP::Num<N>& lhs, const MP::Num<M>& rhs);

template <int N> auto operator<=>(const MP::Num<N> lhs, const std::int64_t rhs);

template <int N> auto operator<=>(const std::int64_t lhs, const MP::Num<N> rhs);

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

template <int N> MP::Num<N> operator*(const MP::Num<N>& lhs, std::uint32_t rhs);

template <int N, int M>
MP::Num<template_max<N, M>()> operator/(const MP::Num<N>& lhs,
                                        const MP::Num<M>& rhs);

template <int N> MP::Num<N> mod_10(const MP::Num<N>& lhs);
