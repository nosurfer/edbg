#include "ptrace_configure.h"

#include <cerrno>
#include <stdlib.h>

#include <stdexcept>
#include <system_error>

#include <sys/ptrace.h>
#include <sys/wait.h>

void
ptrace_attach(pid_t pid)
{
  pid_t status = 0;
  errno = 0; // zeroed errno to check it in future

  long res = ptrace(PTRACE_ATTACH, pid, nullptr, nullptr);
  if (res == -1 && errno != 0) {
    // sanity check of ptrace result and errno value (if error actually occured) 
    int saved_errno = errno;
    throw std::system_error(
        saved_errno,
        std::generic_category(),
        // this function return a reference to an error_category object
        // used to indentify error conditions that correspond to errno codes
        // that object have a lot of methods
        "PTRACE_ATTACH failed"
    );
  }

  status = waitpid(pid, &status, 0);
  if (status == -1) {
    int saved_errno = errno;
    throw std::system_error(
        saved_errno,
        std::generic_category(),
        "WAITPID failed"
    );
  }

  if (!WIFSTOPPED(status)) {
    // sanity check that traced process has stoped
    throw std::runtime_error("process didn't stop after attaching");
  }
}

void
ptrace_detach(pid_t pid)
{
  errno = 0;

  long res = ptrace(PTRACE_DETACH, pid, nullptr, nullptr);
  if (res == -1 && errno != 0) {
    int saved_errno = errno;
    throw std::system_error(
        saved_errno,
        std::generic_category(),
        "PTRACE_DETACH failed"
    );
  }
}





