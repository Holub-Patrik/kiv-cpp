#include "Drawing.h"
#include "Canvas.h"
#include "Primitives.h"
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>

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

// this a tool that will help us later (>ᴗ•)
using primitive_ret = std::tuple<std::unique_ptr<Drawable>, std::string>;
using transformation_ret = std::optional<std::string>;
using arg_type = const std::vector<std::string>&;
using primitive_func = std::function<primitive_ret(arg_type)>;
using transformation_func =
    std::function<transformation_ret(arg_type, std::unique_ptr<Canvas>&)>;

// semi-robust check if argument is numeric
// doesn't handle non base 10 arguments
bool is_number(const std::string& s) {
  if (s == "") {
    return false;
  }
  bool sign = false;
  bool dot = false;
  auto it = s.begin();

  if (*it == '+' || *it == '-') {
    sign = true;
    it++;
  }

  for (; it != s.end(); it++) {
    if (!std::isdigit(*it)) {
      if (!dot && *it == '.') {
        dot = true;
        continue;
      }

      return false;
    }
  }

  return true;
}

static primitive_ret generate_line(arg_type parts) {
  double x1, y1, x2, y2;
  if (parts.size() < 4) {
    return {nullptr, "Malformed line\nNot enough args"};
  }

  for (const auto& part : parts) {
    if (!is_number(part)) {
      return {nullptr,
              std::format("Malformed line\nArgument isn't a whole number: {}",
                          part)};
    }
  }

  x1 = std::stod(parts[0]);
  y1 = std::stod(parts[1]);
  x2 = std::stod(parts[2]);
  y2 = std::stod(parts[3]);

  return {std::move(std::unique_ptr<Drawable>(new Line(x1, y1, x2, y2))), ""};
}

static primitive_ret generate_circle(arg_type parts) {
  double x, y, r;
  if (parts.size() < 3) {
    return {nullptr, "Malformed line\nNot enough args"};
  }

  for (const auto& part : parts) {
    if (!is_number(part)) {
      return {nullptr,
              std::format("Malformed line\nArgument isn't a whole number: {}",
                          part)};
    }
  }

  x = std::stod(parts[0]);
  y = std::stod(parts[1]);
  r = std::stod(parts[2]);

  return {std::move(std::unique_ptr<Drawable>(new Circle(x, y, r))), ""};
}

static primitive_ret generate_rect(arg_type parts) {
  double x, y, width, height;
  if (parts.size() < 4) {
    return {nullptr, "Malformed line\nNot enough args"};
  }

  for (const auto& part : parts) {
    if (!is_number(part)) {
      return {nullptr,
              std::format("Malformed line\nArgument isn't a whole number: {}",
                          part)};
    }
  }

  x = std::stod(parts[0]);
  y = std::stod(parts[1]);
  width = std::stod(parts[2]);
  height = std::stod(parts[3]);

  return {std::move(std::unique_ptr<Drawable>(new Rect(x, y, width, height))),
          ""};
}

static transformation_ret generate_translate(arg_type parts,
                                             std::unique_ptr<Canvas>& c) {
  double x, y;
  if (parts.size() < 2) {
    return "Malformed line\nNot enough args";
  }

  for (const auto& part : parts) {
    if (!is_number(part)) {
      return std::format("Malformed line\nArgument ins't a number: {}", part);
    }
  }

  x = std::stod(parts[0]);
  y = std::stod(parts[1]);

  *c *= TranslateMatrix{x, y};

  return std::nullopt;
}

static transformation_ret generate_scale(arg_type parts,
                                         std::unique_ptr<Canvas>& c) {
  double x, y, a;
  if (parts.size() < 3) {
    return "Malformed line\nNot enough args";
  }

  for (const auto& part : parts) {
    if (!is_number(part)) {
      return std::format("Malformed line\nArgument isn't a number: {}", part);
    }
  }

  x = std::stod(parts[0]);
  y = std::stod(parts[1]);
  a = std::stod(parts[2]);

  *c *= TranslateMatrix(-x, -y);
  *c *= ScaleMatrix(a);
  *c *= TranslateMatrix(x, y);

  return std::nullopt;
}

static transformation_ret generate_rot(arg_type parts,
                                       std::unique_ptr<Canvas>& c) {
  double x, y, f;
  if (parts.size() < 3) {
    return "Malformed line\nNot enough args";
  }

  for (const auto& part : parts) {
    if (!is_number(part)) {
      return std::format("Malformed line\nArgument isn't number: {}", part);
    }
  }

  x = std::stod(parts[0]);
  y = std::stod(parts[1]);
  f = std::stod(parts[2]);

  *c *= TranslateMatrix(-x, -y);
  *c *= RotateMatrix(f);
  *c *= TranslateMatrix(x, y);

  return std::nullopt;
}

// this is where we use the tool
// basically a hashmap with generate functions that are matched against their
// appropriate line
// if someone wants to add a new primitive, just write a primitive function and
// add it here !
static std::unordered_map<std::string, primitive_func> primitives_gen{
    {"line", generate_line},
    {"circle", generate_circle},
    {"rect", generate_rect},
};

static std::unordered_map<std::string, transformation_func> transform_gen{
    {"translate", generate_translate},
    {"scale", generate_scale},
    {"rotate", generate_rot},
};

// take a copy as I will be making sure it's trimmed
std::optional<std::string> Drawing::ParseLine(const std::string& line_in) {
  // this splits the line
  std::istringstream buffer(line_in);
  std::vector<std::string> parts{std::istream_iterator<std::string>(buffer),
                                 std::istream_iterator<std::string>()};
  for (int i = 0; i < parts.size(); i++) {
    if (parts[i].starts_with("#")) {
      parts.erase(parts.begin() + i, parts.end());
      break;
    }
  }

  // entire line is erased aka entire line is comment
  if (parts.size() == 0) {
    return std::nullopt;
  }

  // take out the command so that the only args passed are the numbers
  std::string command = parts[0];
  parts.erase(parts.begin());

  auto search_primitives = primitives_gen.find(command);
  if (search_primitives != primitives_gen.end()) {
    std::unique_ptr<Drawable> obj;
    std::string err_msg;

    std::tie(obj, err_msg) = search_primitives->second(parts);
    if (obj) {
      m_canvas->Add(std::move(obj));
    } else {
      return err_msg;
    }
  }

  auto search_transf = transform_gen.find(command);
  if (search_transf != transform_gen.end()) {
    auto res = search_transf->second(parts, m_canvas);
    if (res) {
      return *res;
    }
  }

  return std::nullopt;
}
