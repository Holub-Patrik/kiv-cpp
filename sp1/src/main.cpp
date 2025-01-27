#include "Canvas.h"
#include "Drawing.h"
#include "Primitives.h"
#include <cstdlib>
#include <memory>
#include <utility>

void test_draw() {
  std::unique_ptr<Drawable> rect_1(new Rect(40, 50, 100, 30));
  std::unique_ptr<Drawable> rect_1_orig(new Rect(40, 50, 100, 30));
  std::unique_ptr<Drawable> circ_1(new Circle(40, 50, 20));
  std::unique_ptr<Drawable> circ_1_orig(new Circle(40, 50, 20));
  std::unique_ptr<Drawable> line_1(new Line(40, 50, 100, 30));
  std::unique_ptr<Drawable> line_1_orig(new Line(40, 50, 100, 30));

  std::unique_ptr<Drawable> rect_2(new Rect(40, 50, 100, 30));
  std::unique_ptr<Drawable> rect_2_orig(new Rect(40, 50, 100, 30));
  std::unique_ptr<Drawable> circ_2(new Circle(40, 50, 20));
  std::unique_ptr<Drawable> circ_2_orig(new Circle(40, 50, 20));
  std::unique_ptr<Drawable> line_2(new Line(40, 50, 100, 30));
  std::unique_ptr<Drawable> line_2_orig(new Line(40, 50, 100, 30));

  TranslateMatrix mov1{-40, -50};
  ScaleMatrix scale{3.3};
  RotateMatrix test_rot{22.5};
  TranslateMatrix mov2{40, 50};

  SVGCanvas svg{800, 600};
  PGMCanvas bmp{800, 600};

  svg.Add(std::move(rect_1));
  svg.Add(std::move(circ_1));
  svg.Add(std::move(line_1));
  svg *= mov1;
  svg *= scale;
  svg *= test_rot;
  svg *= mov2;
  svg.Add(std::move(rect_1_orig));
  svg.Add(std::move(circ_1_orig));
  svg.Add(std::move(line_1_orig));

  bmp.Add(std::move(rect_2));
  bmp.Add(std::move(circ_2));
  bmp.Add(std::move(line_2));
  bmp *= mov2;
  bmp *= test_rot;
  bmp *= scale;
  bmp *= mov1;
  bmp.Add(std::move(rect_2_orig));
  bmp.Add(std::move(circ_2_orig));
  bmp.Add(std::move(line_2_orig));

  svg.Save("test.svg");
  bmp.Save("test.pgm");
}

int main(int argc, char* argv[]) {
  Drawing prog("input.txt", "test_input.pgm", 800, 600);
  auto ret_val = prog.run();
  return ret_val;
}
