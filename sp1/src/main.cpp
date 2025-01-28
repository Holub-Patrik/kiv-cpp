#include "Canvas.h"
#include "Drawing.h"
#include "Primitives.h"
#include <array>
#include <cstdlib>
#include <iostream>
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
  if (argc < 4) {
    std::cout << "Not enough arguments passed\nArgs are: <input-file> "
                 "<output-file.[pgm|svg]> <AxB>"
              << std::endl;
    return EXIT_FAILURE;
  }

  // parse the 4th argument
  std::string dims_str{argv[3]};
  std::size_t x_index = dims_str.find_first_of("x");
  std::array<std::string, 2> dims_parts = {dims_str.substr(0, x_index),
                                           dims_str.substr(x_index + 1)};
  std::array<int, 2> dims;
  try {
    dims[0] = std::atoi(dims_parts[0].c_str());
    dims[1] = std::atoi(dims_parts[1].c_str());
  } catch (std::exception ex) {
    std::cout << "Dimens need to be numeric, whole numbers" << std::endl;
    return EXIT_FAILURE;
  }

  auto err = Drawing::validate_input(argv[1], argv[2]);
  if (err) {
    std::cout << *err << std::endl;
    return EXIT_FAILURE;
  }

  Drawing prog(argv[1], argv[2], dims[0], dims[1]);
  auto ret_val = prog.run();
  if (ret_val > 0) {
    std::cout << ret_val << '\n' << "OK" << std::endl;
    return EXIT_SUCCESS;
  } else {
    return EXIT_FAILURE;
  }
}
