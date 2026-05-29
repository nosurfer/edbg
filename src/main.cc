#include "ptrace_session.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>

int main(int argc, char *argv[])
{
  
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <pid>" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  PtraceSession s = PtraceSession(argv[1]);
  s.get_regs();
  getchar();
  
  std::exit(EXIT_SUCCESS);
}
