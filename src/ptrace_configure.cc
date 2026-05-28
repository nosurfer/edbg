#include "ptrace_configure.h"

#include <cerrno>
#include <cstdlib>
#include <string>
#include <stdexcept>
#include <system_error>

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>

#include <stdlib.h>
#include <unistd.h>

void ptrace_attach(pid_t pid)
{
  int status = 0;
  pid_t result;
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

  result = waitpid(pid, &status, 0);
  if (result == -1) {
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

pid_t ptrace_fork(const std::string& pathname)
{
  pid_t pid, result;
  int status = 0;
  pid = fork();

  if (pid == 0) {
    ptrace(PTRACE_TRACEME, 0, nullptr, nullptr);
    // If the current program is being ptraced, a SIGTRAP
    // signal is sent to it after a successful execve().
    execl(pathname.c_str(), pathname.c_str(), (char*)nullptr);
    // Immediately terminates the program abnormally.
    std::abort();
  }
  
  result = waitpid(pid, &status, 0);
  if (result == -1) {
    int saved_errno = errno;
    throw std::system_error(
        saved_errno,
        std::generic_category(),
        "WAITPID failed"
    );
  }

  if (!WIFSTOPPED(status)) {
    // sanity check that traced process has stoped
    throw std::runtime_error("process didn't stop after fork");
  }

  return pid;
}

void ptrace_detach(pid_t pid)
{
  errno = 0;

  long res = ptrace(PTRACE_DETACH, pid, nullptr, nullptr);
  if (res == -1 || errno != 0) {
    int saved_errno = errno;
    throw std::system_error(
        saved_errno,
        std::generic_category(),
        "PTRACE_DETACH failed"
    );
  }
}

struct user_regs_struct ptrace_getregs(pid_t pid)
{
  struct user_regs_struct regs;
  errno = 0;

  long res = ptrace(PTRACE_GETREGS, pid, nullptr, &regs);
  if (res == -1 || errno != 0) {
    int saved_errno = errno;
    throw std::system_error(
        saved_errno,
        std::generic_category(),
        "PTRACE_GETREGS failed"
    );
  }

  return regs;
}
