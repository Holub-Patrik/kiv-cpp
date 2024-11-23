module;
#include <filesystem>
#include <fstream>
#include <print>
#include <regex>
#include <string>
#include <tuple>
#include <vector>

export module FileFinder;

export class FileFinderException : std::exception {
public:
  explicit FileFinderException(const char* reason) : _msg(reason) {}
  explicit FileFinderException(const std::string reason) : _msg(reason) {}

  virtual ~FileFinderException() noexcept {}

  virtual const char* what() const noexcept { return _msg.c_str(); }

protected:
  std::string _msg;
};

// return all files with given extension and their size
export std::vector<std::tuple<std::string, ulong>>
FindWithExtension(const std::string& extension, const std::string& path) {
  // std::filesystem::path fs_path = std::filesystem::relative(path);
  std::filesystem::path fs_path{path};
  std::vector<std::tuple<std::string, ulong>> files_with_size{};

  fs_path.make_preferred();

  if (!std::filesystem::is_directory(fs_path)) {
    throw FileFinderException{"Given path isn't a directory."};
  }

  for (auto& item : std::filesystem::recursive_directory_iterator(fs_path)) {
    if (std::filesystem::is_directory(item)) {
      continue;
    }

    if (item.path().extension() != extension) {
      continue;
    }

    ulong f_size = item.file_size();
    std::string f_name = item.path().lexically_normal().string();

    std::tuple<std::string, ulong> f_info{f_name, f_size};

    files_with_size.push_back(f_info);
  }

  return files_with_size;
}

// return a list of files containing the given regex word
export std::vector<std::filesystem::path>
FindInFiles(const std::string& pattern, const std::string& path) {
  // std::filesystem::path fs_path = std::filesystem::relative(path);
  std::filesystem::path fs_path{path};
  std::vector<std::filesystem::path> files_to_search{};
  std::vector<std::filesystem::path> files_containing{};
  std::regex rg(pattern);

  fs_path.make_preferred();

  if (!std::filesystem::is_directory(fs_path)) {
    throw FileFinderException{"Given path isn't a directory."};
  }

  for (auto& item : std::filesystem::recursive_directory_iterator(fs_path)) {
    if (std::filesystem::is_directory(item)) {
      continue;
    }

    if (item.path().extension() != ".txt") {
      continue;
    }

    files_to_search.push_back(item);
  }

  std::string line;
  // implement searching
  for (auto& file_path : files_to_search) {
    std::ifstream file{file_path.string()};

    if (!file.is_open()) {
      throw FileFinderException{
          std::format("Couldn't access file: {}", file_path.string())};
    }

    bool found = false;
    while (std::getline(file, line)) {
      if (std::regex_search(line, rg)) {
        found = true;
        break;
      }
    }

    if (found) {
      files_containing.push_back(file_path);
    }
  }

  return files_containing;
}
