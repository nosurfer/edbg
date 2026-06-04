#include "debugger.cc"

#include <cstdlib>

int main()
{
  Debugger dbg;
  dbg.run();
  std::exit(EXIT_SUCCESS);
}
