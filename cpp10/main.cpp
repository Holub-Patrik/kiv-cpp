#include <filesystem>
#include <iostream>
#include <print>

using namespace std::chrono_literals;

int main(int argc, char* argv[]) {
  std::println("Starting ...");

  std::filesystem::path downloads("/home\\holubpat\\Downloads");
  std::println(std::cout, "{}", downloads.string());

  for (auto dir : std::filesystem::directory_iterator(downloads)) {
    std::cout << dir.path().filename() << std::endl;
  }

  return 0;
}
