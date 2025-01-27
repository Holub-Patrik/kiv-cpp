#include "Canvas.h"
#include "Primitives.h"
#include <cstdlib>
#include <memory>
#include <utility>

int main(int argc, char* argv[]) {
  std::unique_ptr<Drawable> p_r1(new Rect(40, 50, 100, 30));

  std::unique_ptr<Drawable> p_l3(new Line(0, 0, 200, 100));
  std::unique_ptr<Drawable> p_l4(new Line(0, 200, 100, 0));
  std::unique_ptr<Drawable> p_c2(new Circle(200, 100, 50));
  std::unique_ptr<Drawable> p_r2(new Rect(40, 50, 100, 30));

  ScaleMatrix test_mat{2};
  TranslateMatrix mov1{-40, -50};
  RotateMatrix test_rot{22.5};
  TranslateMatrix mov2{40, 50};

  SVGCanvas svg{800, 600};
  BMPCanvas bmp{800, 600};

  svg.Add(std::move(p_r1));
  svg *= mov1;
  svg *= test_rot;
  svg *= mov2;

  bmp.Add(std::move(p_l3));
  bmp.Add(std::move(p_l4));
  bmp.Add(std::move(p_c2));
  bmp.Add(std::move(p_r2));

  svg.Save("test.svg");
  bmp.Save("test.pgm");

  return EXIT_SUCCESS;
}
