#include "Canvas.h"
#include <fstream>
#include <memory>
#include <optional>

class Drawing final {
private:
  std::fstream m_file_in;
  std::string m_file_out;
  std::unique_ptr<Canvas> m_canvas;

public:
  Drawing() = delete;
  ~Drawing();

  Drawing(std::string, std::string, int,
          int); // file_in, file_out, width, height
  // should be called before calling the constructor
  static std::optional<std::string> validate_input(std::string, std::string);
  int ParseFile();
  std::optional<std::string> ParseLine(const std::string&);
  int run();
};
