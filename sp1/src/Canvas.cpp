#include "Canvas.h"
#include "Primitives.h"
#include <algorithm>
#include <cstdint>
#include <execution>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

Canvas::Canvas()
    : m_objects(std::vector<std::unique_ptr<Drawable>>{}), m_width(0),
      m_height(0) {}
Canvas::Canvas(int width, int height)
    : m_objects(std::vector<std::unique_ptr<Drawable>>{}), m_width(width),
      m_height(height) {}

Canvas& Canvas::operator*=(const TMatrix& transform) {
  std::for_each(std::execution::par_unseq, m_objects.begin(), m_objects.end(),
                [this, &transform](auto& obj_ptr) { *obj_ptr *= transform; });
  return *this;
}

Canvas& Canvas::operator<<(std::unique_ptr<Drawable>&& object) {
  Add(std::move(object));
  return *this;
}

RasterCanvas::RasterCanvas() : Canvas() {}
RasterCanvas::RasterCanvas(int width, int height)
    : Canvas(width, height), pixel_matrix(std::vector<int>{}) {
  pixel_matrix.reserve(width * height);
}

void RasterCanvas::Add(std::unique_ptr<Drawable>&& obj_ptr) {
  m_objects.push_back(std::move(obj_ptr));
}

VectorCanvas::VectorCanvas() : Canvas() {}
VectorCanvas::VectorCanvas(int width, int height) : Canvas(width, height) {}

void VectorCanvas::Add(std::unique_ptr<Drawable>&& obj_ptr) {
  m_objects.push_back(std::move(obj_ptr));
}

BMPCanvas::BMPCanvas() : RasterCanvas() {}
BMPCanvas::BMPCanvas(int width, int height) : RasterCanvas(width, height) {}

void BMPCanvas::Save(std::string file_path) {
  std::fstream out_file(file_path, std::fstream::out | std::fstream::trunc);

  out_file << std::format("P5\n{}\n{}\n1\n", m_width, m_height);

  std::for_each(std::execution::par_unseq, m_objects.begin(), m_objects.end(),
                [this](auto&& obj_ptr) {
                  const auto& temp = dynamic_cast<PGMDrawable&>(*obj_ptr);
                  auto pixels = temp.DrawPGM();
                  for (const auto& pixel : pixels) {
                    if (pixel[0] > m_width || pixel[1] > m_height) {
                      continue;
                    }

                    pixel_matrix[pixel[1] * m_width + pixel[0]] = 1;
                  }
                });

  constexpr std::uint8_t white = 0;
  constexpr std::uint8_t black = 1;
  for (int y = 0; y < m_height; y++) {
    for (int x = 0; x < m_width; x++) {
      if (pixel_matrix[y * m_width + x]) {
        out_file << white;
      } else {
        out_file << black;
      }
    }
  }
}

SVGCanvas::SVGCanvas()
    : end_stack(std::vector<std::string>{}), VectorCanvas() {}
SVGCanvas::SVGCanvas(int width, int height) : VectorCanvas(width, height) {}

void SVGCanvas::Save(std::string file_path) {
  std::fstream out_file(file_path, std::fstream::out | std::fstream::binary |
                                       std::fstream::trunc);
  out_file << std::format("<svg version=\"1.1\" "
                          "width=\"{}\" height=\"{}\" "
                          "style=\"background-color:white\" "
                          "xmlns=\"http://www.w3.org/2000/svg\">",
                          m_width, m_height)
           << std::endl;

  std::for_each(
      m_objects.begin(), m_objects.end(), [&out_file](auto&& obj_ptr) {
        const SVGDrawable& temp = dynamic_cast<SVGDrawable&>(*obj_ptr);
        out_file << temp.DrawSVG() << std::endl;
      });

  out_file << std::format("</svg>") << std::endl;
}
