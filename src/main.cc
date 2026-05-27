#include "spdlog/cfg/env.h"
#include "spdlog/spdlog.h"

#include "ptrace_session.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>

int
main(int argc, char *argv[])
{
  spdlog::cfg::load_env_levels();

  SPDLOG_TRACE("Program start");
  
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <pid>" << std::endl;
    return EXIT_FAILURE;
  }

  PtraceSession s = PtraceSession(argv[1]);
  s.get_regs();
  getchar();
  
  return EXIT_SUCCESS;
}
