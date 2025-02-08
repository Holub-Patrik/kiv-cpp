#include "Primitives.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <format>
#include <iostream>
#include <utility>
#include <vector>

Pos::Pos() : m_repr({0}) {}
Pos::Pos(double x, double y) : m_repr(std::array<double, 2>{x, y}) {}

Pos::Pos(const Pos& other) : m_repr(other.m_repr) {}
Pos::Pos(Pos&& other) : m_repr(std::move(other.m_repr)) {}

Pos& Pos::operator=(const Pos& other) {
  if (this == &other)
    return *this;

  Pos temp(other);
  std::swap(m_repr, temp.m_repr);

  return *this;
}

Pos& Pos::operator=(Pos&& other) {
  Pos temp(std::move(other));
  std::swap(m_repr, temp.m_repr);
  return *this;
}

Pos& Pos::operator+=(const Pos& other) {
  m_repr[0] += other[0];
  m_repr[1] += other[1];

  return *this;
}

Pos& Pos::operator-=(const Pos& other) {
  m_repr[0] -= other[0];
  m_repr[1] -= other[1];

  return *this;
}

Pos& Pos::operator*=(const Pos& other) {
  m_repr[0] *= other[0];
  m_repr[1] *= other[1];

  return *this;
}

Pos& Pos::operator*=(double mult) {
  m_repr[0] *= mult;
  m_repr[1] *= mult;

  return *this;
}

Pos& Pos::operator*=(const TMatrix& other) {
  *this = std::move(*this * other);

  return *this;
}

Pos& Pos::operator/=(const Pos& other) {
  m_repr[0] /= other[0];
  m_repr[1] /= other[1];

  return *this;
}

Pos& Pos::operator/=(double div) {
  m_repr[0] /= div;
  m_repr[1] /= div;

  return *this;
}

double Pos::operator[](const max_index<2> auto index) const {
  return m_repr[index];
}

double Pos::operator[](const vec_pos_str auto index) const {
  if (index == "x") {
    return m_repr[0];
  } else {
    return m_repr[1];
  }
}

Pos operator+(const Pos& lhs, const Pos& rhs) {
  auto res = lhs;
  res += rhs;
  return res;
}

Pos operator-(const Pos& lhs, const Pos& rhs) {
  auto res = lhs;
  res -= rhs;
  return res;
}

Pos operator*(const Pos& lhs, const Pos& rhs) {
  auto res = lhs;
  res *= rhs;
  return res;
}

Pos operator*(const Pos& lhs, double mult) {
  auto res = lhs;
  res *= mult;
  return res;
}

Pos operator/(const Pos& lhs, const Pos& rhs) {
  auto res = lhs;
  res /= rhs;
  return res;
}

Pos operator/(const Pos& lhs, double mult) {
  auto res = lhs;
  res /= mult;
  return res;
}

TMatrix::TMatrix() : m_repr({0}) {}

std::array<double, 3> TMatrix::operator[](max_index<3> auto index) const {
  return m_repr[index];
}

std::array<double, 3>& TMatrix::operator[](max_index<3> auto index) {
  return m_repr[index];
}

TMatrix operator*(const TMatrix& lhs, const TMatrix& rhs) {
  TMatrix res;

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      res[i][j] = 0;

      for (int k = 0; k < 3; k++) {
        res[i][j] += lhs[i][k] * rhs[k][j];
      }
    }
  }

  return res;
}

TMatrix& TMatrix::operator*=(const TMatrix& other) {
  *this = *this * other;
  return *this;
}

RotateMatrix::RotateMatrix(double angle) {
  constexpr double PI = M_PI; // type control, but very useless
  const double rad = (angle * PI) / 180;
  m_repr[0][0] = std::cos(rad);
  m_repr[1][1] = std::cos(rad);

  m_repr[0][1] = -std::sin(rad);
  m_repr[1][0] = std::sin(rad);

  m_repr[2][2] = 1;
}

TranslateMatrix::TranslateMatrix(double x, double y) {
  m_repr[0][0] = 1;
  m_repr[1][1] = 1;
  m_repr[2][2] = 1;

  m_repr[0][2] = x;
  m_repr[1][2] = y;
}

ScaleMatrix::ScaleMatrix(double scale) {
  m_repr[0][0] = scale;
  m_repr[1][1] = scale;

  m_repr[2][2] = 1;
}

IdentityMatrix::IdentityMatrix() {
  m_repr[0][0] = 1;
  m_repr[1][1] = 1;
  m_repr[2][2] = 1;
}

// the order shouldn't matter, it will always be done the same way, I just
// didn't want to care during writing code
Pos operator*(const Pos& lhs, const TMatrix& rhs) {
  const std::array<double, 3> vec_3 = {lhs[0], lhs[1], 1};
  std::array<double, 3> res{0};

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      res[i] += rhs[j][i] * vec_3[j];
    }
  }

  return Pos(res[0], res[1]);
}

// the order doesn't matter, it will always be done the same way, I just didn't
// want to care during writing code
Pos operator*(const TMatrix& lhs, const Pos& rhs) {
  const std::array<double, 3> vec_3 = {rhs[0], rhs[1], 1};
  std::array<double, 3> res{0};

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      res[i] += lhs[i][j] * vec_3[j];
    }
  }

  return Pos(res[0], res[1]);
}

Drawable::Drawable() : m_pos({0, 0}) {}
Drawable::~Drawable() = default;

SVGDrawable::~SVGDrawable() = default;
SVGDrawable::SVGDrawable() = default;

PGMDrawable::~PGMDrawable() = default;
PGMDrawable::PGMDrawable() = default;

Line::~Line() = default;
Line& Line::operator*=(const TMatrix& transform) {
  m_transform = transform * m_transform;
  return *this;
}

Line::Line(double x1, double y1, double x2, double y2) {
  m_transform = IdentityMatrix();
  m_pos = {x1, y1};
  m_end = {x2, y2};
}

Line::Line(Pos start, Pos end) {
  m_transform = IdentityMatrix();
  m_pos = start;
  m_end = {end["x"], end["y"]};
}

std::string Line::DrawSVG() const {
  return std::format("<line x1=\"{}\" y1=\"{}\" x2=\"{}\" y2=\"{}\" "
                     "transform=\"matrix({} {} {} {} {} {})\" "
                     "style=\"stroke:black;stroke-width:2\"/>",
                     m_pos["x"], m_pos["y"], m_end["x"], m_end["y"],
                     m_transform[0][0], m_transform[1][0], m_transform[0][1],
                     m_transform[1][1], m_transform[0][2], m_transform[1][2]);
}

std::vector<std::array<int, 2>> Line::DrawPGM() const {
  std::vector<std::array<int, 2>> res;

  const auto start_transf = m_transform * m_pos;
  const auto end_transf = m_transform * m_end;

  int x0 = static_cast<int>(start_transf["x"]);
  int y0 = static_cast<int>(start_transf["y"]);
  int x1 = static_cast<int>(end_transf["x"]);
  int y1 = static_cast<int>(end_transf["y"]);

  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2; /* error value e_xy */

  for (;;) { /* loop */
    res.push_back({x0, y0});
    if (x0 == x1 && y0 == y1)
      break;
    e2 = 2 * err;
    if (e2 >= dy) {
      err += dy;
      x0 += sx;
    } /* e_xy+e_x > 0 */
    if (e2 <= dx) {
      err += dx;
      y0 += sy;
    } /* e_xy+e_y < 0 */
  }

  return std::move(res);
}

Circle::~Circle() = default;
Circle::Circle(double x, double y, double r) : m_r(r) {
  m_transform = IdentityMatrix();
  m_pos = {x, y};
}
Circle::Circle(Pos pos, double r) : m_r(r) {
  m_transform = IdentityMatrix();
  m_pos = pos;
}

Circle& Circle::operator*=(const TMatrix& transform) {
  m_transform = transform * m_transform;
  return *this;
}

std::string Circle::DrawSVG() const {
  return std::format(
      "<circle cx=\"{}\" cy=\"{}\" r=\"{}\" "
      "transform=\"matrix({} {} {} {} {} {})\" "
      "style=\"fill:white;fill-opacity:0;stroke:black;stroke-width:2\"/>",
      m_pos["x"], m_pos["y"], m_r, m_transform[0][0], m_transform[1][0],
      m_transform[0][1], m_transform[1][1], m_transform[0][2],
      m_transform[1][2]);
}

std::vector<std::array<int, 2>> Circle::DrawPGM() const {
  std::vector<std::array<int, 2>> res;

  const auto new_pos = m_transform * m_pos;
  const auto rad_vec = Pos{m_r, 0};
  auto scale_and_rot = m_transform;
  // extract just the scale and rotate, so no translation happens
  scale_and_rot[0][2] = 0;
  scale_and_rot[1][2] = 0;

  scale_and_rot[2][0] = 0;
  scale_and_rot[2][1] = 0;

  scale_and_rot[2][2] = 1;
  // scale the vector
  const auto rad_vec_scaled = scale_and_rot * rad_vec;

  // all of above done so that this equation holds true sqrt(x^2 + y^2)
  auto radius = (std::sqrt(std::pow(rad_vec_scaled[1], 2) +
                           std::pow(rad_vec_scaled[0], 2)));

  int xm = static_cast<int>(new_pos["x"]);
  int ym = static_cast<int>(new_pos["y"]);
  int r = static_cast<int>(radius);
  int x = -r, y = 0, err = 2 - 2 * r; /* II. Quadrant */

  do {
    res.push_back({xm - x, ym + y}); /*   I. Quadrant */
    res.push_back({xm - y, ym - x}); /*  II. Quadrant */
    res.push_back({xm + x, ym - y}); /* III. Quadrant */
    res.push_back({xm + y, ym + x}); /*  IV. Quadrant */
    r = err;
    if (r <= y)
      err += ++y * 2 + 1; /* e_xy+e_y < 0 */
    if (r > x || err > y)
      err += ++x * 2 + 1; /* e_xy+e_x > 0 or no 2nd y-step */
  } while (x < 0);

  return std::move(res);
}

Rect::~Rect() = default;
Rect::Rect(double x1, double x2, double width, double height)
    : m_width(width), m_height(height) {
  m_transform = IdentityMatrix();
  m_pos = {x1, x2};
  // all other rectangle points in clockwise order, saved for PGM render
  m_points = {m_pos + Pos{width, 0}, m_pos + Pos{width, height},
              m_pos + Pos{0, height}};
}
Rect::Rect(Pos pos, double width, double height) {
  m_transform = IdentityMatrix();
  m_pos = pos;
}

Rect& Rect::operator*=(const TMatrix& transform) {
  m_transform = transform * m_transform;
  return *this;
}

std::string Rect::DrawSVG() const {
  return std::format(
      "<rect x=\"{}\" y=\"{}\" width=\"{}\" height=\"{}\" "
      "transform=\"matrix({} {} {} {} {} {})\" "
      "style=\"fill:white;fill-opacity:0;stroke:black;stroke-width:2\"/>",
      m_pos["x"], m_pos["y"], m_width, m_height, m_transform[0][0],
      m_transform[1][0], m_transform[0][1], m_transform[1][1],
      m_transform[0][2], m_transform[1][2]);
}

std::vector<std::array<int, 2>> Rect::DrawPGM() const {
  std::vector<std::array<int, 2>> res;

  const auto new_pos = m_transform * m_pos;
  std::array<Pos, 3> new_points = m_points;
  for (auto& new_point : new_points) {
    new_point = m_transform * new_point;
  }

  // rectangle is 4 lines
  std::array<Line, 4> rect_lines = {
      Line(new_pos, new_points[0]),
      Line(new_points[0], new_points[1]),
      Line(new_points[1], new_points[2]),
      Line(new_points[2], new_pos),
  };

  // draw each line
  std::for_each(rect_lines.begin(), rect_lines.end(), [&res](auto line) {
    for (auto pixel : line.DrawPGM()) {
      res.push_back(pixel);
    }
  });

  return res;
}
