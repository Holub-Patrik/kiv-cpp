#include <array>
#include <cstdint>
template <uint64_t max_bytes> class BigNumber {
private:
  // this means I get 4 bytes for each position
  std::array<uint32_t, max_bytes / 4> repr;
  uint64_t size = max_bytes;

public:
  BigNumber() {}

  BigNumber operator+(BigNumber const& other) {}
};

int main(int argc, char* argv[]) { return 0; }
