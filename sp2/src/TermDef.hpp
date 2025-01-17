#include "Number.hpp"
#include <array>

// +, -, *, /, !

template <int N> class Term final {
private:
  std::array<MP::Num<N>, 5> bank;

public:
  Term() : bank(std::array<MP::Num<N>, 5>{}) {}
  ~Term() {}

  bool split_eq(const std::string& eq_string, std::array<std::string, 3>& eq);
};
