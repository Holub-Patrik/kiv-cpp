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
