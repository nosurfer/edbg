#include "ptrace_session.h"

#include <cstdlib>
#include <iostream>

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <pid>" << std::endl;
  }

  /* todo
   * 1. add logs (extern lib)
   * 2. more comments everywhere...
   * 3. breakpoint handler
   * 4. temporary makefile */
  
  return 0;
}
