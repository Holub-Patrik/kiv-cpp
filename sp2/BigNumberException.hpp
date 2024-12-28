#include "BigIntDef.hpp"
#include <exception>
#include <string>

template <size_t size> class BigNumberException : std::exception {
public:
  explicit BigNumberException<size>(const char* reason) : _msg(reason) {}
  explicit BigNumberException<size>(const std::string reason) : _msg(reason) {}
  explicit BigNumberException<size>(const char* reason, BigInt<size> number)
      : _msg(reason), _num(number) {}
  explicit BigNumberException<size>(const std::string reason,
                                    BigInt<size> number)
      : _msg(reason), _num(number) {}

  virtual ~BigNumberException<size>() noexcept {}
  virtual const char* what() const noexcept { return _msg.c_str(); }
  operator BigInt<size>() const { return _num; }

private:
  std::string _msg;
  BigInt<size> _num{0};
};
