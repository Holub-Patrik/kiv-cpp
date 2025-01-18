#include "NumberDef.hpp"
#include <array>
#include <optional>
#include <tuple>

// +, -, *, /, !

enum operators {
  ADD,
  SUB,
  MUL,
  DIV,
  FAC,
};
using op = enum operators;

template <int N> using eq_type = std::tuple<MP::Num<N>, op, MP::Num<N>>;

template <int N> class Term final {
private:
  std::array<MP::Num<N>, 5> bank;

public:
  Term() : bank(std::array<MP::Num<N>, 5>{}) {}
  ~Term() {}

  std::optional<std::array<std::string, 3>>
  split_eq(const std::string& eq_string);
  std::optional<MP::Num<N>> calculate(const eq_type<N>& eq);
  std::optional<eq_type<N>> parse_numbers(const std::array<std::string, 3>& eq);
  void run();
  void shift_bank(const MP::Num<N>& new_num);
};
