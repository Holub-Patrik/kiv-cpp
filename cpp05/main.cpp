#include <format>
#include <iostream>

int main(int argc, char **argv) {
  std::cout << "Starting ...\n";

  const int num = 312456;

  std::cout << std::format("Pi is {:-^10}", num) << std::endl;

  std::cout << "Ending" << std::endl;
  return EXIT_SUCCESS;
}
