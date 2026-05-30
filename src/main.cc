#include "debugger.cc"

#include <cstdlib>
#include <iostream>

int main(int argc, char *argv[])
{
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <pathname>" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  Debugger dbg(argv[1]);
  dbg.run();

  std::exit(EXIT_SUCCESS);
}
