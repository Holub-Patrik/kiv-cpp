#include <string>
#include <vector>

class Command {
public:
  Command() {}
  virtual ~Command() = 0;

  virtual void Execute() = 0;
};
