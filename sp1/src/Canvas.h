#include "Primitives.h"
#include <string>
#include <vector>

class Canvas {
protected:
  std::vector<Drawable> m_objects;

public:
  Canvas() {} // empty as the base class interacts as interface
  virtual ~Canvas() = 0;

  virtual void Save() = 0;
  virtual void Add(Drawable) = 0;
};

class RasterCanvas : public Canvas {
public:
  RasterCanvas();
  virtual ~RasterCanvas() override;

  virtual void Add(Drawable) override final;
};

class BMPCanvas final : RasterCanvas {
public:
  BMPCanvas();
  virtual ~BMPCanvas() override final;

  virtual void Save() override final;
};

class VectorCanvas : public Canvas {
public:
  VectorCanvas();
  virtual ~VectorCanvas() override;
  virtual void Add(Drawable) override final;
};

class SVGCanvas final : public VectorCanvas {
public:
  SVGCanvas();
  virtual ~SVGCanvas() override final;

  virtual void Save() override final;
};
