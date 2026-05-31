#include "debugger.cc"

#include <cstdlib>

int main(int argc, char *argv[])
{
  Debugger dbg;
  dbg.run();
  std::exit(EXIT_SUCCESS);
}
