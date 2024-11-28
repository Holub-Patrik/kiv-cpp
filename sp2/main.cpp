#include <algorithm>
#include <array>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <exception>
#include <execution>
#include <format>
#include <limits>
#include <numbers>
#include <print>
#include <ranges>

// the complete insanity done in operator+() will be done for muptiplication too
// the adition of the carry array will have to be done twice
// First time to fix add the carry, and then add the carries, that may have been
// generated again

template <typename T>
static consteval T template_max(const T& lhs, const T& rhs) {
  if (lhs > rhs) {
    return lhs;
  } else {
    return rhs;
  }
} // namespace BigNumbertemplate<typenameT>staticconstevalT
  // template_max(constT&lhs,constT&rhs) template_max(constT&lhs,constT&rhs)

template <typename T>
concept SizeParam = std::convertible_to<T, uint64_t> && requires(T t) {
  { t + t } -> std::convertible_to<T>;
  { t - t } -> std::convertible_to<T>;
  { t % t } -> std::convertible_to<T>;
  { t / t } -> std::convertible_to<T>;
};

static consteval uint64_t size_ceil(SizeParam auto val, SizeParam auto div) {
  const uint64_t temp = div - val % div;
  return (val + temp) / div;
}

template <typename T> static consteval uint64_t const_ceil(const T& num) {
  uint64_t temp = static_cast<uint64_t>(num);
  if (temp < num) {
    return temp + 1;
  } else {
    return temp;
  }
}

class BigNumberException : std::exception {
public:
  explicit BigNumberException(const char* reason) : _msg(reason) {}
  explicit BigNumberException(const std::string reason) : _msg(reason) {}

  virtual ~BigNumberException() noexcept {}

  virtual const char* what() const noexcept { return _msg.c_str(); }

private:
  std::string _msg;
};

template <uint64_t size> class BigInt {
private:
  // this means I get 4 bytes for each position
  // all operations I need to implement can be done per parts
  // also dividing by 10 can be done per parts (for converting into decimal)
  const uint64_t bytes = size;
  std::array<uint32_t, size_ceil(size, 4)> repr;

public:
  // default without constructor
  BigInt() : repr(std::array<uint32_t, size_ceil(size, 4)>{}) {
    // std::println("Array length: {}", repr.size());
  }
  // initialize given an internal represantation to be used
  BigInt(std::array<uint32_t, size_ceil(size, 4)> init_arr) : repr(init_arr) {
    // std::println("Array length: {}", repr.size());
  }
  // initialize using a number
  // allow only for initializing using uint32_t
  BigInt(uint32_t num) : repr(std::array<uint32_t, size_ceil(size, 4)>{}) {
    repr[0] = num;
    // std::println("Array length: {}", repr.size());
  }

  ~BigInt() {}

  constexpr const uint64_t max_bytes() const noexcept { return bytes; };
  constexpr const std::array<uint32_t, size_ceil(size, 4)>&
  get_repr() const noexcept {
    return repr;
  }

  constexpr bool is_negative() {
    auto last_elem = repr.back();
    return (last_elem >> 31) & 1;
  }

  BigInt<size> get_complement() const {
    auto complement_repr = repr;
    std::for_each(std::execution::par_unseq, complement_repr.begin(),
                  complement_repr.end(), [](auto& elem) { elem = !elem; });

    BigInt<size> complement{complement_repr};
    complement += BigInt<size>{1};
    return complement;
  }

  template <uint64_t other_size>
  BigInt<size> operator+=(BigInt<other_size> rhs) {
    if (other_size > size) {
      throw BigNumberException("Cannot do this!");
    }

    const auto& rhs_arr = rhs.get_repr();

    auto& res_arr = repr;
    const auto& work_arr = rhs.get_repr();
    std::array<uint32_t, size> carry_arr{};
    auto index_gen = std::views::iota(0);
    auto index = std::views::counted(index_gen.begin(), size);

    std::for_each(std::execution::par_unseq, index.begin(), index.end(),
                  [&carry_arr, &res_arr, work_arr](auto index) {
                    const uint64_t temp =
                        static_cast<uint64_t>(res_arr[index]) +
                        static_cast<uint64_t>(work_arr[index]);
                    res_arr[index] = static_cast<uint32_t>(temp);
                    carry_arr[index] = temp >> 32;
                  });

    std::for_each(std::execution::par_unseq, index.begin(), index.end(),
                  [&carry_arr, &res_arr](auto index) {
                    if (index == 0) {
                      return;
                    }
                    res_arr[index] += carry_arr[index - 1];
                  });

    return *this;
  }

  operator std::string() const {
    // division by ten seems to always shift the number by 3 bits
    // this I can pick the 3 lower bits of the next byte, shift them to top
    // and add them to the current number
    // chars count eq:
    // n * ln(P) / ln(Q)t
    // Q = dest chars (dec)
    // P = source chars (bin)
    // I will only be converting to decadic so
    //  ln(P) = ln(2)
    //  ln(Q) = ln(10)
    const uint64_t chars_count =
        const_ceil(size * 8 * std::numbers::ln2 / std::numbers::ln10);
    std::array<char, chars_count> chars{};

    size_t index = 0;
    while (repr[0] > 0) {
      chars[index] = repr[0] % 10;
      repr[0] /= 10;
      for (size_t shift_index = 1; shift_index < repr.size(); ++shift_index) {
        uint64_t lower_three = repr[shift_index] & 7; // take out lower 3 bits
        lower_three <<= 29; // shift them to the highest 3 bits
        repr[shift_index - 1] += lower_three; // add them to the elem before
        repr[shift_index] >>= 3; // shift by 3 bits since they were moved lower
      }

      ++index;
    }
  }
};

template <uint64_t lhs_size, uint64_t rhs_size>
BigInt<template_max(lhs_size, rhs_size)>
operator+(BigInt<lhs_size> const& lhs, BigInt<rhs_size> const& rhs) {
  constexpr const uint64_t resulting_size = template_max(lhs_size, rhs_size);
  const auto& rhs_arr = rhs.get_repr();
  const auto& lhs_arr = lhs.get_repr();

  std::array<uint32_t, size_ceil(resulting_size, 4)> res_arr{};
  std::array<uint32_t, size_ceil(resulting_size, 4)> work_arr{};
  std::array<uint32_t, size_ceil(resulting_size, 4)> carry_arr{};
  auto index_gen = std::views::iota(0);
  auto index = std::views::counted(index_gen.begin(), resulting_size / 4);

  // copy the shorter array into the res_arr
  // copy the longer one to work array to be worked on
  // I would love to just somehow alias the work array and not copy it, but I do
  // not know how
  if (lhs_size > rhs_size) {
    std::copy(rhs_arr.begin(), rhs_arr.end(), res_arr.begin());
    std::copy(lhs_arr.begin(), lhs_arr.end(), work_arr.begin());
  } else {
    std::copy(lhs_arr.begin(), lhs_arr.end(), res_arr.begin());
    std::copy(rhs_arr.begin(), rhs_arr.end(), work_arr.begin());
  }

  std::for_each(std::execution::par_unseq, index.begin(), index.end(),
                [&carry_arr, &res_arr, work_arr](auto index) {
                  const uint64_t temp = static_cast<uint64_t>(res_arr[index]) +
                                        static_cast<uint64_t>(work_arr[index]);
                  res_arr[index] = static_cast<uint32_t>(temp);
                  carry_arr[index] = temp >> 32;
                });

  std::for_each(std::execution::par_unseq, index.begin(), index.end(),
                [&carry_arr, &res_arr](auto index) {
                  if (index == 0) {
                    return;
                  }
                  res_arr[index] += carry_arr[index - 1];
                });

  return BigInt<resulting_size>(res_arr);
}

template <uint64_t lhs_size, uint64_t rhs_size>
BigInt<template_max(lhs_size, rhs_size)>
operator-(BigInt<lhs_size> const& rhs, BigInt<rhs_size> const& lhs) {
  lhs.get_complement();
  return rhs + lhs;
}

template <uint64_t lhs_size, uint64_t rhs_size>
auto operator<=>(BigInt<lhs_size> const& rhs, BigInt<rhs_size> const& lhs) {
  auto lhs_arr = lhs.get_repr();
  auto rhs_arr = rhs.get_repr();
  size_t lhs_index = 0;
  size_t rhs_index = 0;

  for (auto it = lhs_arr.end(); it != lhs_arr.begin();) {
    --it;

    if (*it != 0) {
      lhs_index = std::distance(lhs_arr.begin(), it);
      break;
    }
  }

  for (auto it = rhs_arr.end(); it != rhs_arr.begin();) {
    --it;

    if (*it != 0) {
      lhs_index = std::distance(rhs_arr.begin(), it);
      break;
    }
  }

  std::println("{} ? {}", lhs_index, rhs_index);
  if (lhs_index > rhs_index) {
    return lhs_index <=> rhs_index;
  } else if (lhs_index < rhs_index) {
    return lhs_index <=> rhs_index;
  } else {
    return lhs_arr[lhs_index] <=> rhs_arr[rhs_index];
  }
}

int main(int argc, char* argv[]) {
  BigInt<40> bg1{std::numeric_limits<uint32_t>::max()};
  BigInt<50> bg2{std::numeric_limits<uint32_t>::max()};
  BigInt<90> test{std::numeric_limits<uint32_t>::max()};

  auto bg3 = bg1 - bg2;
  auto bg4 = bg1 + bg2;
  bg4 += bg2;
  std::println("{}\n", bg1 < bg3);
  for (auto& elem : bg3.get_repr()) {
    std::println("{}", elem);
  }
  return 0;
}
