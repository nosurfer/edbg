#include "spdlog/cfg/env.h"
#include "spdlog/spdlog.h"

#include "ptrace_session.h"

#include <cstdlib>
#include <iostream>

int
main(int argc, char *argv[])
{
  spdlog::cfg::load_env_levels();

  SPDLOG_TRACE("Entering main()");
  SPDLOG_DEBUG("argc = {}", argc);
  SPDLOG_INFO("Program started");
  SPDLOG_WARN("No config file found, using defaults");
  SPDLOG_ERROR("Failed to open file: {}", "data.txt");
  SPDLOG_CRITICAL("Fatal error, exiting");
  
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
