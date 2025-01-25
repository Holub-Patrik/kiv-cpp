#include "Canvas.h"
#include "Primitives.h"
#include <fstream>

int main(int argc, char* argv[]) {
  Line l1{0, 0, 1, 1};
  Line l2{0, 1, 1, 0};

  SVGCanvas svg{};
  BMPCanvas bmp{};

  std::fstream svg_file("test.svg");
  std::fstream bmp_file("test.bmp");

  svg.Add(l1);
  svg.Add(l2);

  bmp.Add(l1);
  bmp.Add(l2);

  svg.Save(std::move(svg_file));
  bmp.Save(std::move(bmp_file));

  return 0;
}
