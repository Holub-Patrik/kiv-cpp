#pragma once
#include "DataTypes.h"
#include <vector>

class Drawable {
protected:
  Pos m_pos;
  Color m_color;
  std::vector<int> m_args;

public:
  Drawable();
  virtual ~Drawable();
};

class Line : public Drawable {
public:
  Line() = delete;
  virtual ~Line() override;

  Line(double, double, double, double);
  Line(Pos, Pos);
};
