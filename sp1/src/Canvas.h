#pragma once
#include "Primitives.h"
#include <algorithm>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

class Canvas {
protected:
  int m_width;
  int m_height;
  std::vector<std::unique_ptr<Drawable>> m_objects;

public:
  Canvas();
  Canvas(int, int);
  virtual ~Canvas() = default;

  virtual void Save(std::string) = 0;
  virtual void Add(std::unique_ptr<Drawable>&&) = 0;

  virtual Canvas& operator*=(const TMatrix&);
  virtual Canvas& operator<<(std::unique_ptr<Drawable>&&) final;
};

class RasterCanvas : public Canvas {
protected:
  std::vector<int> pixel_matrix;

public:
  RasterCanvas();
  RasterCanvas(int, int);

  virtual ~RasterCanvas() override = default;

  virtual void Save(std::string) override = 0;
  virtual void Add(std::unique_ptr<Drawable>&&) override final;
};

class BMPCanvas final : public RasterCanvas {
public:
  BMPCanvas();
  BMPCanvas(int, int);
  virtual ~BMPCanvas() override final = default;

  virtual void Save(std::string) override final;
};

class VectorCanvas : public Canvas {
public:
  VectorCanvas();
  VectorCanvas(int, int);

  virtual ~VectorCanvas() override = default;

  virtual void Save(std::string) override = 0;
  virtual void Add(std::unique_ptr<Drawable>&&) override final;
};

class SVGCanvas final : public VectorCanvas {
private:
  std::vector<std::string> end_stack;

public:
  SVGCanvas();
  SVGCanvas(int, int);
  virtual ~SVGCanvas() override final = default;

  virtual SVGCanvas& operator*=(const TMatrix&) override;
  virtual void Save(std::string) override final;
};
