#pragma once
#include <array>
#include <string>
#include <vector>

struct Pos {
  int X, Y;
};
using Pos = struct Pos;

// defines what all of them have to have in terms of variables
class Drawable {
protected:
  Pos m_pos;
  std::vector<int> m_args; // different args for different drawables

public:
  Drawable() = delete; // cannot be instantiated
  virtual ~Drawable();
};

// defines that said drawable can be rendered as SVG
class SVGDrawable : virtual public Drawable {
public:
  SVGDrawable() = delete; // cannot be instantiated
  virtual ~SVGDrawable();

  virtual std::string DrawSVG() const = 0;
};

// defines that said drawable can be rendered as BMP
class BMPDrawable : virtual public Drawable {
public:
  BMPDrawable() = delete; // cannot be instantiated
  virtual ~BMPDrawable();

  virtual std::vector<std::array<int, 2>> DrawPixels() const = 0;
};

class Line : public SVGDrawable, public BMPDrawable {
public:
  Line() = delete;
  virtual ~Line() override;

  Line(int, int, int, int);
  Line(Pos, Pos);

  virtual std::string DrawSVG() const override;
  virtual std::vector<std::array<int, 2>> DrawPixels() const override;
};

class Circle : public SVGDrawable, public BMPDrawable {
public:
  Circle() = delete;
  virtual ~Circle() override;

  Circle(int, int, int);
  Circle(Pos, int);

  virtual std::string DrawSVG() const override;
  virtual std::vector<std::array<int, 2>> DrawPixels() const override;
};

class Rect : public SVGDrawable, public BMPDrawable {
public:
  Rect() = delete;
  virtual ~Rect() override;

  Rect(int, int, int, int);
  Rect(Pos, int, int);

  virtual std::string DrawSVG() const override;
  virtual std::vector<std::array<int, 2>> DrawPixels() const override;
};
