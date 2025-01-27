#pragma once
#include <array>
#include <concepts>
#include <cstddef>
#include <string>
#include <vector>

template <typename T>
concept vec_pos_num = std::convertible_to<T, std::size_t> && requires(T a) {
  a < 2;
  a > 0;
};

template <typename T>
concept vec_pos_str = std::convertible_to<T, std::string> && requires(T a) {
  static_cast<std::string>(a) == "x" || static_cast<std::string>(a) == "y";
};

template <typename T, std::size_t N>
concept max_index = std::convertible_to<T, std::size_t> && requires(T a) {
  a < N;
  a > 0;
};

class TMatrix {
protected:
  std::array<std::array<double, 3>, 3> m_repr;

public:
  TMatrix();
  virtual ~TMatrix() = default;

  TMatrix& operator*=(const TMatrix&);
  std::array<double, 3> operator[](max_index<3> auto index) const;
};

class Pos final {
private:
  std::array<double, 2> m_repr;

public:
  Pos();
  ~Pos() = default;

  Pos(double, double);

  Pos(const Pos&);
  Pos(Pos&&);

  Pos& operator=(const Pos&);
  Pos& operator=(Pos&&);

  Pos& operator+=(const Pos&);
  Pos& operator-=(const Pos&);
  Pos& operator*=(const Pos&);
  Pos& operator*=(double);
  Pos& operator*=(const TMatrix&);
  Pos& operator/=(const Pos&);
  Pos& operator/=(double);
  double operator[](const max_index<2> auto) const;
  double operator[](const vec_pos_str auto) const;
};

Pos operator+(const Pos&, const Pos&);
Pos operator-(const Pos&, const Pos&);
Pos operator*(const Pos&, const Pos&);
Pos operator*(const Pos&, double);
Pos operator*(const Pos&, const TMatrix&);
Pos operator*(const TMatrix&, const Pos&);
Pos operator/(const Pos&, const Pos&);
Pos operator/(const Pos&, double);

TMatrix operator*(const TMatrix&, const TMatrix&);

class RotateMatrix final : public TMatrix {
public:
  RotateMatrix() = delete;
  virtual ~RotateMatrix() override final = default;

  RotateMatrix(double);
};

class TranslateMatrix final : public TMatrix {
public:
  TranslateMatrix() = delete;
  virtual ~TranslateMatrix() override final = default;

  TranslateMatrix(double, double);
};

class ScaleMatrix final : public TMatrix {
public:
  ScaleMatrix() = delete;
  virtual ~ScaleMatrix() override final = default;

  ScaleMatrix(double);
  ScaleMatrix(double, double);
};

// defines what all of them have to have in terms of variables
class Drawable {
protected:
  Pos m_pos; // first position point

public:
  Drawable();
  virtual ~Drawable();

  virtual Drawable& operator*=(const TMatrix&) = 0;
};

// defines that said drawable can be rendered as SVG
class SVGDrawable : virtual public Drawable {
public:
  SVGDrawable();
  virtual ~SVGDrawable();

  virtual std::string DrawSVG() const = 0;
  virtual SVGDrawable& operator*=(const TMatrix&) = 0;
};

// defines that said drawable can be rendered as BMP
class PGMDrawable : virtual public Drawable {
public:
  PGMDrawable();
  virtual ~PGMDrawable();

  virtual std::vector<std::array<int, 2>> DrawPGM() const = 0;
};

class Line : public SVGDrawable, public PGMDrawable {
private:
  Pos m_end;

public:
  Line() = delete; // do not allow for empty values
  virtual ~Line() override;

  Line(double, double, double, double);
  Line(Pos, Pos);

  virtual std::string DrawSVG() const override;
  virtual std::vector<std::array<int, 2>> DrawPGM() const override;
  virtual Line& operator*=(const TMatrix&) override;
};

class Circle : public SVGDrawable, public PGMDrawable {
private:
  double m_r;

public:
  Circle() = delete; // do not allow for empty values
  virtual ~Circle() override;

  Circle(double, double, double);
  Circle(Pos, double);

  virtual std::string DrawSVG() const override;
  virtual std::vector<std::array<int, 2>> DrawPGM() const override;
  virtual Circle& operator*=(const TMatrix&) override;
};

class Rect : public SVGDrawable, public PGMDrawable {
private:
  std::array<Pos, 3> m_points;
  double m_width;
  double m_height;

public:
  Rect() = delete; // do not allow for empty values
  virtual ~Rect() override;

  Rect(double, double, double, double);
  Rect(Pos, double, double);

  virtual std::string DrawSVG() const override;
  virtual std::vector<std::array<int, 2>> DrawPGM() const override;
  virtual Rect& operator*=(const TMatrix&) override;
};
