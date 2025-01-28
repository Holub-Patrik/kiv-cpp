#include "Drawing.h"
#include "Canvas.h"
#include "Primitives.h"
#include <algorithm>
#include <cctype>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

Drawing::~Drawing() = default;

Drawing::Drawing(std::string file_in, std::string file_out, int width,
                 int height) {

  m_file_in = std::fstream(file_in, std::fstream::in);
  std::filesystem::path fout_path{file_out};
  std::string extension = fout_path.extension();

  // this is wrong. It would be better to have a Saver interface, which would
  // call proper save and stuff
  if (extension == ".svg") {
    m_canvas = std::unique_ptr<Canvas>(new SVGCanvas(width, height));
    m_file_out = file_out;
  } else if (extension == ".pgm") {
    m_canvas = std::unique_ptr<Canvas>(new PGMCanvas(width, height));
    m_file_out = file_out;
  }
}

std::optional<std::string> Drawing::validate_input(std::string file_in,
                                                   std::string file_out) {
  if (!std::filesystem::exists(file_in)) {
    return "Input file doesn't exist";
  }

  std::fstream try_open{file_in, std::fstream::in};
  if (!try_open.is_open()) {
    return "File couldn't be opened";
  }

  std::string extension = std::filesystem::path{file_out}.extension();
  if (extension == ".svg" || extension == ".pgm") {
    return std::nullopt;
  } else {
    return std::format(
        "Output file extension not recognized: {}\nValid are: .svg | .pgm",
        extension);
  }

  return std::nullopt;
}

int Drawing::ParseFile() {
  std::string line;
  int lines_parsed = 0;
  while (std::getline(m_file_in, line)) {
    auto ret = ParseLine(line);
    if (ret) {
      std::cout << *ret << std::endl;
      return -1;
    }
    lines_parsed++;
  }

  return lines_parsed;
}

int Drawing::run() {
  int lines_parsed = ParseFile();
  if (lines_parsed > 0) {
    m_canvas->Save(m_file_out);
  }

  return lines_parsed;
}

// take a copy as I will be making sure it's trimmed
std::optional<std::string> Drawing::ParseLine(std::string line_in) {
  // first trim from left
  line_in.erase(line_in.begin(),
                std::find_if(line_in.begin(), line_in.end(),
                             [](unsigned char c) { return !std::isspace(c); }));

  std::vector<std::string> parts;
  std::stringstream line{line_in};
  std::string temp;

  // split by spaces
  while (std::getline(line, temp, ' ')) {
    parts.push_back(temp);
  }

  if (parts[0].starts_with("#")) {
    return std::nullopt;
  }

  if (parts[0] == "line") {
    double x1, y1, x2, y2;

    if (parts.size() < 5) {
      return std::format("Malformed line (not enough args): {}", line_in);
    }

    try {
      x1 = std::stod(parts[1]);
      y1 = std::stod(parts[2]);
      x2 = std::stod(parts[3]);
      y2 = std::stod(parts[4]);
    } catch (std::exception ex) {
      return std::format("Malformed line (non number argument): {}", line_in);
    }

    m_canvas->Add(std::unique_ptr<Drawable>(new Line(x1, y1, x2, y2)));
  }

  if (parts[0] == "circle") {
    double x, y, r;

    if (parts.size() < 4) {
      return std::format("Malformed line (not enough args): {}", line_in);
    }

    try {
      x = std::stod(parts[1]);
      y = std::stod(parts[2]);
      r = std::stod(parts[3]);
    } catch (std::exception ex) {
      return std::format("Malformed line (non number argument): {}", line_in);
    }

    m_canvas->Add(std::unique_ptr<Drawable>(new Circle(x, y, r)));
  }

  if (parts[0] == "rect") {
    double x, y, width, height;

    if (parts.size() < 5) {
      return std::format("Malformed line (not enough args): {}", line_in);
    }

    try {
      x = std::stod(parts[1]);
      y = std::stod(parts[2]);
      width = std::stod(parts[3]);
      height = std::stod(parts[4]);
    } catch (std::exception ex) {
      return std::format("Malformed line (non number argument): {}", line_in);
    }

    m_canvas->Add(std::unique_ptr<Drawable>(new Rect(x, y, width, height)));
  }

  if (parts[0] == "translate") {
    double x, y;

    if (parts.size() < 3) {
      return std::format("Malformed line (not enough args): {}", line_in);
    }

    try {
      x = std::stod(parts[1]);
      y = std::stod(parts[2]);
    } catch (std::exception ex) {
      return std::format("Malformed line (non number argument): {}", line_in);
    }

    *m_canvas *= TranslateMatrix(x, y);
  }

  if (parts[0] == "rotate") {
    double x, y, a;

    if (parts.size() < 4) {
      return std::format("Malformed line (not enough args): {}", line_in);
    }

    try {
      x = std::stod(parts[1]);
      y = std::stod(parts[2]);
      a = std::stod(parts[3]);
    } catch (std::exception ex) {
      return std::format("Malformed line (non number argument): {}", line_in);
    }

    *m_canvas *= TranslateMatrix(-x, -y);
    *m_canvas *= RotateMatrix(a);
    *m_canvas *= TranslateMatrix(x, y);
  }

  if (parts[0] == "scale") {
    double x, y, a;

    if (parts.size() < 4) {
      return std::format("Malformed line (not enough args): {}", line_in);
    }

    try {
      x = std::stod(parts[1]);
      y = std::stod(parts[2]);
      a = std::stod(parts[3]);
    } catch (std::exception ex) {
      return std::format("Malformed line (non number argument): {}", line_in);
    }

    *m_canvas *= TranslateMatrix(-x, -y);
    *m_canvas *= ScaleMatrix(a);
    *m_canvas *= TranslateMatrix(x, y);
  }

  return std::nullopt;
}
