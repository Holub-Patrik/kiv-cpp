#include "Primitives.h"
#include <array>
#include <sstream>
#include <vector>

std::string Line::DrawSVG() const {
  std::stringstream s;
  s << "<line ";
  s << "x1=\"" << m_pos.X << "\" ";
  s << "y1=\"" << m_pos.Y << "\" ";
  s << "x2=\"" << m_args[0] << "\" ";
  s << "y2=\"" << m_args[1] << "\" ";
  s << "style=\"stroke:black;stroke-width:14\" />";
  return s.str();
}

std::vector<std::array<int, 2>> Line::DrawPixels() const {
  Pos end = {static_cast<int>(m_args[0]), static_cast<int>(m_args[1])};
  std::vector<std::array<int, 2>> res;

  int x0 = m_pos.X;
  int y0 = m_pos.Y;
  int x1 = m_args[0];
  int y1 = m_args[1];

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

  return res;
}

std::string Circle::DrawSVG() const {}

std::vector<std::array<int, 2>> Circle::DrawPixels() const {}

std::string Rect::DrawSVG() const {}

std::vector<std::array<int, 2>> Rect::DrawPixels() const {}
