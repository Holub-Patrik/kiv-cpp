#include "TermDef.hpp"
#include <algorithm>
#include <array>
#include <cctype>
#include <regex>
#include <string>

// regex pattern (\w+!?)(?:([\+\-\*\/])(.+))?

template <int N>
bool Term<N>::split_eq(const std::string& eq_string,
                       std::array<std::string, 3>& eq) {
  std::string trimmed = eq_string;
  trimmed.erase(std::remove_if(trimmed.begin(), trimmed.end(),
                               [](unsigned char x) { return std::isspace(x); }),
                trimmed.end());

  std::regex eq_regex("(\\w+!?)(?:([\\+\\-\\*\\/])(.+))?");
  std::smatch matches;
  bool valid_eq = std::regex_match(trimmed, matches, eq_regex);

  if (!valid_eq) {
    return false;
  }

  // skip first match, that's the eq itself
  int index = 0;
  for (auto match = ++matches.begin(); match != matches.end(); match++) {
    eq[index] = *match;
    index++;
  }

  return true;
}
