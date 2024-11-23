#include <print>

constexpr double MiB = 1048576;

import FileFinder;

int main(int argc, char* argv[]) {
  for (int i = 1; i < argc; ++i) {
    std::string arg_string{argv[i]};

    if (arg_string == "ff") {
      if (i + 2 >= argc || i + 1 >= argc) {
        std::println("Wrong argument count");
        return 0;
      }

      std::string extension{argv[i + 1]};
      std::string directory{argv[i + 2]};

      std::println("\033[3mFinding files with extension {} in {}\033[0m",
                   extension, directory);

      try {
        auto items = FindWithExtension(extension, directory);

        for (auto& item : items) {
          auto& [name, size] = item;

          double size_MiB = static_cast<double>(size) / MiB;
          std::println("{} | {:6.3f} MiB", name, size_MiB);
        }
      } catch (FileFinderException ex) {
        std::println("{}", ex.what());
        return 1;
      }
    }

    if (arg_string == "fg") {
      if (i + 2 >= argc || i + 1 >= argc) {
        std::println("Wrong argument count");
        return 0;
      }

      std::string pattern{argv[i + 1]};
      std::string directory{argv[i + 2]};

      std::println("\033[3mLooking for files containing {} in {}\033[0m",
                   pattern, directory);

      try {
        auto files = FindInFiles(pattern, directory);

        if (files.size() == 0) {
          std::println("No files found.");
        }

        for (auto& file : files) {
          std::println("{}", file.lexically_normal().string());
        }
      } catch (FileFinderException ex) {
        std::println("{}", ex.what());
        return 1;
      }
    }
  }
  return 0;
}
