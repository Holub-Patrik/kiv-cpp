#include <cmath>
#include <iostream>

// 2D vector (x,y)
class Vector2D final {
  double x;
  double y;

public:
  Vector2D(double x, double y) : x(x), y(y) {};
  ~Vector2D() = default;

  double get_x() const { return x; }
  double get_y() const { return y; }
  double get_size() const { return sqrt(x * x + y * y); }

  // access into vector like into an array,
  // returning NAN when accessing outside the vector
  double operator[](const int &idx) const {
    switch (idx) {
    case 'a':
    case 'x':
    case 0:
      return x;
      break;
    case 'b':
    case 'y':
    case 1:
      return y;
      break;
    default:
      throw std::invalid_argument("Invalid array idx");
      break;
    }
  }

  // allow accessing into the vector using chars 'x', 'y'
  // returning NAN for other chars
  Vector2D operator+(const Vector2D &vec) const {
    return {x + vec.get_x(), y + vec.get_y()};
  }
  Vector2D operator+=(const Vector2D &vec) {
    x += vec['x'];
    y += vec['y'];

    return *this;
  }

  Vector2D operator*(const double &n) const { return {x * n, y * n}; }
  Vector2D operator*=(const double &n) {
    x *= n;
    y *= n;
    return *this;
  }

  double operator*(const Vector2D &vec) const {
    return x * vec['x'] + y * vec['y'];
  }

  auto operator<=>(const Vector2D &vec) const {
    return this->get_size() <=> vec.get_size();
  }
};

std::ostream &operator<<(std::ostream &os, const Vector2D &vec) {
  os << "[" << vec['x'] << "," << vec['y'] << "]";
  return os;
}

int main(int argc, char *argv[]) {
  Vector2D vec_a(1.0, 2.0);
  Vector2D vec_b(2.0, 3.0);

  const auto vec_a_mult = vec_a *= 2.0;
  const auto vec_sum = vec_a + vec_b;
  const auto scalar = vec_a * vec_b;

  std::cout << "Vector a: " << vec_a << std::endl;
  std::cout << "Vector b: " << vec_b << std::endl;
  std::cout << "Sum of a and b: " << vec_sum << std::endl;
  std::cout << "Dot product: " << scalar << std::endl;

  const auto order = vec_a <=> vec_b;
  std::cout << "Vec_a "
            << (order < 0   ? "<"
                : order > 0 ? ">"
                            : "==")
            << " Vec_b" << std::endl;

  return 0;
}
