#pragma once
#include "NumberDef.hpp"

#include <exception>
#include <string>

namespace MP {
template <int N> class Exception : public std::exception {
public:
  explicit Exception(const char* reason) : _msg(reason) {}
  explicit Exception(const std::string reason) : _msg(reason) {}
  explicit Exception(const char* reason, Num<N> number)
      : _msg(reason), _num(number) {}
  explicit Exception(const std::string reason, Num<N> number)
      : _msg(reason), _num(number) {}

  virtual ~Exception() noexcept {}
  virtual const char* what() const noexcept { return _msg.c_str(); }
  operator Num<N>() const { return _num; }

private:
  std::string _msg;
  Num<N> _num{0};
};
} // namespace MP
