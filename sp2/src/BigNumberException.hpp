#pragma once
#include "BigIntDef.hpp"

#include <exception>
#include <string>

template <size_t size> class BigNumberException : public std::exception {
public:
  explicit BigNumberException(const char* reason) : _msg(reason) {}
  explicit BigNumberException(const std::string reason) : _msg(reason) {}
  explicit BigNumberException(const char* reason, BigInt<size> number)
      : _msg(reason), _num(number) {}
  explicit BigNumberException(const std::string reason, BigInt<size> number)
      : _msg(reason), _num(number) {}

  virtual ~BigNumberException() noexcept {}
  virtual const char* what() const noexcept { return _msg.c_str(); }
  operator BigInt<size>() const { return _num; }

private:
  std::string _msg;
  BigInt<size> _num{0};
};
