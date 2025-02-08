#include "NumberDef.hpp"
#include "NumberEx.hpp"
#include "TermDef.hpp"
#include <algorithm>
#include <array>
#include <cctype>
#include <functional>
#include <iostream>
#include <optional>
#include <regex>
#include <string>
#include <tuple>

// template <int N>
// auto add_lam =
//     std::async(std::launch::deferred,
//                [](const MP::Num<N>& lhs,
//                   const MP::Num<N>& rhs) -> std::optional<MP::Num<N>> {
//                  MP::Num<N> res;
//                  try {
//                    res = lhs + rhs;
//                    return res;
//                  } catch (MP::Exception<N> ex) {
//                    return std::nullopt;
//                  }
//                });
// template <int N> std::array<decltype(add_lam<N>), 4> func_lam_arr{};

template <int N>
static std::optional<MP::Num<N>> add(const MP::Num<N>& lhs,
                                     const MP::Num<N>& rhs) {
  MP::Num<N> res;
  try {
    res = lhs + rhs;
    return res;
  } catch (MP::Exception<N> ex) {
    return std::nullopt;
  }
}

template <int N>
static std::optional<MP::Num<N>> sub(const MP::Num<N>& lhs,
                                     const MP::Num<N>& rhs) {
  MP::Num<N> res;
  try {
    res = lhs - rhs;
    return res;
  } catch (MP::Exception<N> ex) {
    return std::nullopt;
  }
}

template <int N>
static std::optional<MP::Num<N>> mul(const MP::Num<N>& lhs,
                                     const MP::Num<N>& rhs) {
  MP::Num<N> res;
  try {
    res = lhs * rhs;
    return res;
  } catch (MP::Exception<N> ex) {
    return std::nullopt;
  }
}

template <int N>
static std::optional<MP::Num<N>> div(const MP::Num<N>& lhs,
                                     const MP::Num<N>& rhs) {
  MP::Num<N> res;
  try {
    res = lhs / rhs;
    return res;
  } catch (MP::Exception<N> ex) {
    return std::nullopt;
  }
}

template <int N>
static std::optional<MP::Num<N>> fac(const MP::Num<N>& lhs,
                                     const MP::Num<N>& rhs) {
  MP::Num<N> res;
  try {
    res = lhs.factorial();
    return res;
  } catch (MP::Exception<N> ex) {
    return std::nullopt;
  }
}

template <int N>
std::array<std::function<std::optional<MP::Num<N>>(const MP::Num<N>&,
                                                   const MP::Num<N>&)>,
           5>
    func_arr = {add<N>, sub<N>, mul<N>, div<N>, fac<N>};

template <int N>
std::optional<std::array<std::string, 3>>
Term<N>::split_eq(const std::string& eq_string) {
  std::string trimmed = eq_string;
  std::array<std::string, 3> eq;
  trimmed.erase(std::remove_if(trimmed.begin(), trimmed.end(),
                               [](unsigned char x) { return std::isspace(x); }),
                trimmed.end());

  std::regex eq_regex("(\\$?\\w+!?)(?:([\\+\\-\\*\\/])(.+))?");
  std::smatch matches;
  bool valid_eq = std::regex_match(trimmed, matches, eq_regex);

  if (!valid_eq) {
    return std::nullopt;
  }

  // skip first match, that's the eq itself
  int index = 0;
  for (auto match = ++matches.begin(); match != matches.end(); match++) {
    eq[index] = *match;
    index++;
  }

  // if only X! was matched, then set the other 2 parts to empty string
  if (index <= 1) {
    eq[1] = "";
    eq[2] = "";
  }

  return eq;
}

template <int N>
std::optional<eq_type<N>>
Term<N>::parse_numbers(const std::array<std::string, 3>& eq) {
  std::string lhs, oper, rhs;
  std::tie(lhs, oper, rhs) = eq;

  if (lhs == "") {
    return std::nullopt;
  }

  if (oper == "" && rhs == "") { // this means correct factorial eq was found
    if (!lhs.ends_with("!")) {
      return std::nullopt;
    }
    oper = "!";
    lhs.erase(--lhs.end(), lhs.end()); // remove last char which should be '!'
  }

  op enum_op;

  // if somebody presents just one number this will catch it
  switch (oper[0]) {
  case '+':
    enum_op = ADD;
    break;
  case '-':
    enum_op = SUB;
    break;
  case '*':
    enum_op = MUL;
    break;
  case '/':
    enum_op = DIV;
    break;
  case '!':
    enum_op = FAC;
    break;
  default:
    return std::nullopt;
  }

  MP::Num<N> l_num;
  MP::Num<N> r_num;

  // lhs has to always exist
  if (lhs.starts_with("$")) {
    auto bank_index = std::atoi(lhs.substr(1).c_str());
    if (bank_index <= 0 || bank_index > 5) {
      return std::nullopt;
    } else {
      l_num = bank[bank_index - 1];
    }
  } else {
    try {
      l_num = std::move(MP::Num<N>{lhs});
    } catch (MP::Exception<N> ex) {
      return std::nullopt;
    }
  }

  // possibly non-existent (equal to "")
  if (rhs != "" && rhs.starts_with("$")) {
    auto bank_index = std::atoi(rhs.substr(1).c_str());
    if (bank_index <= 0 || bank_index > 5) {
      return std::nullopt;
    } else {
      r_num = bank[bank_index - 1];
    }
  } else {
    try {
      r_num = std::move(MP::Num<N>{rhs});
    } catch (MP::Exception<N> ex) {
      return std::nullopt;
    }
  }

  return std::make_tuple(l_num, enum_op, r_num);
}

template <int N>
std::optional<MP::Num<N>> Term<N>::calculate(const eq_type<N>& eq) {
  MP::Num<N> lhs, rhs;
  op oper;
  std::tie(lhs, oper, rhs) = eq;

  MP::Num<N> res;

  return func_arr<N>[oper](lhs, rhs);
}

template <int N> void Term<N>::run() {
  std::string usr_in;

  if (N == MP::Unlimited) {
    std::cout << "MPCalc (Unlimited precision)" << std::endl;
  } else {
    std::cout << "MPCalc (Limited precision " << N << ")" << std::endl;
  }

  std::cout << "Please enter a simple equation consisting of one operation +, "
               "-, *, / or !."
            << std::endl;

  while (true) {
    std::cout << "> ";
    std::getline(std::cin, usr_in);
    if (usr_in == "exit") {
      std::cout << "Ending operation" << std::endl;
      break;

    } else if (usr_in == "bank") {
      int index = 0;
      for (auto num : bank) {
        std::cout << "$" << index + 1 << " = " << num << std::endl;
        index++;
      }
    } else {
      auto valid_eq = split_eq(usr_in);
      if (!valid_eq) {
        std::cout << "Invalid equation" << std::endl;
        continue;
      }

      auto valid_numbers = parse_numbers(*valid_eq);
      if (!valid_numbers) {
        std::cout << "Invalid numbers" << std::endl;
        continue;
      }

      auto res = calculate(*valid_numbers);
      if (!res) {
        std::cout << "Overflow" << std::endl;
        continue;
      }

      std::cout << "$1 = " << *res << std::endl;
      shift_bank(*res);
    }
  }
}

template <int N> void Term<N>::shift_bank(const MP::Num<N>& new_num) {
  for (int i = bank.size() - 1; i >= 1; --i) {
    bank[i] = std::move(bank[i - 1]);
  }
  bank[0] = new_num;
}
