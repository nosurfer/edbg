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
  errno = 0;

  long res = ptrace(PTRACE_ATTACH, pid, nullptr, nullptr);
  if (res == -1 && errno != 0) {
    int saved_errno = errno;
    throw std::system_error(
        saved_errno,
        std::generic_category(),
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





