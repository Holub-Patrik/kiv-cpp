#include "Canvas.h"

class Transform {
public:
  Transform() = default;
  virtual ~Transform();

  virtual void Use(Canvas) const;
};

class Translate final : public Transform {
private:
  double x;
  double y;

public:
  Translate() = delete;
  virtual ~Translate() override final = default;

  virtual void Use(Canvas) const override final;
};

class Rotate final : public Transform {
private:
  double x;
  double y;
  double r;

public:
  Rotate() = delete;
  virtual ~Rotate() override final = default;

  virtual void Use(Canvas) const override final;
};

class Scale final : public Transform {
private:
  double scale;

public:
  Scale() = delete;
  virtual ~Scale() override final = default;

  virtual void Use(Canvas) const override final;
};
