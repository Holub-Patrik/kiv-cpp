#include "Primitives.h"
#include <algorithm>
#include <array>
#include <format>
#include <vector>

Drawable::Drawable() : m_pos({0, 0}), m_args(2) {}
Drawable::~Drawable() = default;
SVGDrawable::~SVGDrawable() = default;
SVGDrawable::SVGDrawable() = default;
BMPDrawable::~BMPDrawable() = default;
BMPDrawable::BMPDrawable() = default;

Line::~Line() = default;
Circle::~Circle() = default;
Rect::~Rect() = default;

Line::Line(int x1, int y1, int x2, int y2) {
  m_pos = {x1, y1};
  m_args = std::vector<int>{x2, y2};
}

Line::Line(Pos start, Pos end) {
  m_pos = start;
  m_args = std::vector<int>{end.X, end.Y};
}

std::string Line::DrawSVG() const {
  return std::format("<line x1=\"{}\" y1=\"{}\" x2=\"{}\" y2=\"{}\" "
                     "style=\"strije:black;stroke-width:2\">",
                     m_pos.X, m_pos.Y, m_args[0], m_args[1]);
}

std::vector<std::array<int, 2>> Line::DrawPixels() const {
  Pos end = {static_cast<int>(m_args[0]), static_cast<int>(m_args[1])};
  std::vector<std::array<int, 2>> res;

  int x0 = m_pos.X, y0 = m_pos.Y, x1 = m_args[0], y1 = m_args[1];

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

Circle::Circle(int x, int y, int r) {
  m_pos = {x, y};
  m_args = std::vector<int>{r};
}

Circle::Circle(Pos pos, int r) {
  m_pos = pos;
  m_args = std::vector<int>{r};
}

std::string Circle::DrawSVG() const {
  return std::format("<circle cx=\"{}\" cy=\"{}\" r=\"{}\" "
                     "style=\"strije:black;stroke-width:2\">",
                     m_pos.X, m_pos.Y, m_args[0]);
}

std::vector<std::array<int, 2>> Circle::DrawPixels() const {
  std::vector<std::array<int, 2>> res;
  int xm = m_pos.X, ym = m_pos.Y, r = m_args[0];

  int x = -r, y = 0, err = 2 - 2 * r; /* II. Quadrant */
  do {
    res.push_back({xm - x, ym + y});
    res.push_back({xm - y, ym - x});
    res.push_back({xm + x, ym - y});
    res.push_back({xm + y, ym - x});
    r = err;
    if (r <= y)
      err += ++y * 2 + 1; /* e_xy+e_y < 0 */
    if (r > x || err > y)
      err += ++x * 2 + 1; /* e_xy+e_x > 0 or no 2nd y-step */
  } while (x < 0);

  return std::move(res);
}

Rect::Rect(int x1, int x2, int width, int height) {
  m_pos = {x1, x2};
  m_args = std::vector<int>{width, height};
}

Rect::Rect(Pos pos, int width, int height) {
  m_pos = pos;
  m_args = std::vector<int>{width, height};
}

std::string Rect::DrawSVG() const {
  return std::format("<circle x=\"{}\" y=\"{}\" width=\"{}\" height\"{}\" "
                     "style=\"strije:black;stroke-width:2\">",
                     m_pos.X, m_pos.Y, m_args[0], m_args[1]);
}

std::vector<std::array<int, 2>> Rect::DrawPixels() const {
  std::vector<std::array<int, 2>> res;

  // rectangle is 4 lines
  std::array<Line, 4> rect_lines = {
      Line(m_pos.X, m_pos.Y, m_pos.X + m_args[0], m_pos.Y),
      Line(m_pos.X, m_pos.Y, m_pos.X, m_pos.Y + m_args[1]),
      Line(m_pos.X + m_args[0], m_pos.Y, m_pos.X + m_args[0],
           m_pos.Y + m_args[1]),
      Line(m_pos.X, m_pos.Y + m_args[1], m_pos.X + m_args[0],
           m_pos.Y + m_args[1]),
  };

  // draw each line
  std::for_each(rect_lines.begin(), rect_lines.end(), [&res](auto line) {
    for (auto pixel : line.DrawPixels()) {
      res.push_back(pixel);
    }
  });

  return res;
}
