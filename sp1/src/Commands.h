#include "Canvas.h"
#include "Primitives.h"
#include "Transforms.h"

class Command {
  Command() = default;
  virtual ~Command() = 0;

  virtual void Execute(Canvas) = 0;
};

class DrawCommand final : public Command {
private:
  Drawable m_object;

public:
  DrawCommand() = delete;
  virtual ~DrawCommand() override final;

  DrawCommand(Drawable);

  virtual void Execute(Canvas) override final;
};

class TransformCommand final : public Command {
private:
  Transform m_transform;

public:
  TransformCommand() = delete;
  virtual ~TransformCommand() override final;

  TransformCommand(Transform);

  virtual void Execute(Canvas) override final;
};
