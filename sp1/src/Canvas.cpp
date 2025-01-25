#include "Canvas.h"
#include "Primitives.h"
#include <algorithm>
#include <execution>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

Canvas::Canvas()
    : m_objects(std::vector<Drawable>{}), m_width(0), m_height(0) {}
Canvas::Canvas(int width, int height)
    : m_objects(std::vector<Drawable>{}), m_width(width), m_height(height) {}

RasterCanvas::RasterCanvas() : Canvas() {}
RasterCanvas::RasterCanvas(int width, int height)
    : Canvas(width, height), pixel_matrix(std::vector<int>{}) {
  pixel_matrix.reserve(width * height);
}

void RasterCanvas::Add(Drawable object) { m_objects.push_back(object); }

VectorCanvas::VectorCanvas() : Canvas() {}
VectorCanvas::VectorCanvas(int width, int height) : Canvas(width, height) {}

void VectorCanvas::Add(Drawable object) { m_objects.push_back(object); }

BMPCanvas::BMPCanvas() : RasterCanvas() {}
BMPCanvas::BMPCanvas(int width, int height) : RasterCanvas(width, height) {}

void BMPCanvas::Save(std::fstream out_file) {
  out_file << std::format("P5 {} {} 1 \n", m_width, m_height);

  std::for_each(std::execution::par_unseq, m_objects.begin(), m_objects.end(),
                [this](Drawable& object) {
                  auto& temp = dynamic_cast<BMPDrawable&>(object);
                  auto pixels = temp.DrawPixels();
                  for (auto pixel : pixels) {
                    // draw only pixels in viewport
                    if (pixel[0] > m_width || pixel[1] > m_height) {
                      // set pixel in pixel matrix to one
                      pixel_matrix[pixel[1] * m_width + pixel[0]] = 1;
                    }
                  }
                });

  const char white = 1;
  const char black = 0;
  for (int y = 0; y < m_height; y++) {
    for (int x = 0; x < m_width; x++) {
      if (pixel_matrix[y * m_width + x]) {
        out_file << black << " ";
      } else {
        out_file << white;
      }
    }
  }
}

SVGCanvas::SVGCanvas()
    : end_stack(std::vector<std::string>{}), VectorCanvas() {}
SVGCanvas::SVGCanvas(int width, int height) : VectorCanvas(width, height) {}

void SVGCanvas::Save(std::fstream out_file) {
  out_file << std::format("<svg version=\"1.1\" "
                          "width=\"{}\" height=\"{}\""
                          "xmlns=\"http://www.w3.org/2000/svg\">",
                          m_width, m_height)
           << std::endl;

  std::for_each(m_objects.begin(), m_objects.end(),
                [&out_file](Drawable& object) {
                  std::cout << "Casting!" << std::endl;
                  SVGDrawable* temp = (SVGDrawable*)&object;
                  out_file << temp->DrawSVG() << std::endl;
                });

  out_file << std::format("</svg>") << std::endl;
}
