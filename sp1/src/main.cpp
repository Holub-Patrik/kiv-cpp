#include "Drawing.h"
#include <array>
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[]) {
  if (argc < 4) {
    std::cout << "Not enough arguments passed\nArgs are: <input-file> "
                 "<output-file.[pgm|svg]> <AxB>"
              << std::endl;
    return EXIT_FAILURE;
  }

  // parse the 4th argument
  std::string dims_str{argv[3]};
  std::size_t x_index = dims_str.find_first_of("x");
  std::array<std::string, 2> dims_parts = {dims_str.substr(0, x_index),
                                           dims_str.substr(x_index + 1)};
  std::array<int, 2> dims;
  if (!is_number(dims_parts[0]) || !is_number(dims_parts[1])) {
    std::cout << "Dimensions need to be numeric, whole numbers" << std::endl;
    return EXIT_FAILURE;
  }

  dims[0] = std::atoi(dims_parts[0].c_str());
  dims[1] = std::atoi(dims_parts[1].c_str());

  auto err = Drawing::validate_input(argv[1], argv[2]);
  if (err) {
    std::cout << *err << std::endl;
    return EXIT_FAILURE;
  }

  Drawing prog(argv[1], argv[2], dims[0], dims[1]);
  auto ret_val = prog.run();
  if (ret_val > 0) {
    std::cout << ret_val << '\n' << "OK" << std::endl;
    return EXIT_SUCCESS;
  } else {
    return EXIT_FAILURE;
  }
}
