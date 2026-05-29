#include "ptrace_configure.h"

#include <cerrno>
#include <string>
#include <cstdlib>
#include <system_error>

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>

#include <stdlib.h>
#include <unistd.h>

void ptrace_attach(pid_t pid)
{
  // zeroed errno to check it in future
  // sanity check of ptrace result and errno value (if error actually occured) 
  errno = 0;
  long res = ptrace(PTRACE_ATTACH, pid, nullptr, nullptr);
  if (res == -1 && errno != 0) {
    int saved_errno = errno;
    // rewrite this:
    throw std::system_error(
        saved_errno,
        std::generic_category(),
        // this function return a reference to an error_category object
        // used to indentify error conditions that correspond to errno codes
        // that object have a lot of methods
        "PTRACE_ATTACH failed"
    );
  }
}

pid_t ptrace_fork(const std::string& pathname)
{
  pid_t pid;
  // rewrite this:
  pid = fork();

  if (pid == 0) {
    ptrace(PTRACE_TRACEME, 0, nullptr, nullptr);
    // If the current program is being ptraced, a SIGTRAP
    // signal is sent to it after a successful execve().
    execl(pathname.c_str(), pathname.c_str(), (char*)nullptr);
    // Immediately terminates the program abnormally.
    std::_Exit(-1);
  }
  
  return pid;
}

void ptrace_detach(pid_t pid)
{
  errno = 0;
  long res = ptrace(PTRACE_DETACH, pid, nullptr, nullptr);
  if (res == -1 || errno != 0) {
    int saved_errno = errno;
    // rewrite this:
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
    // rewrite this:
    throw std::system_error(
        saved_errno,
        std::generic_category(),
        "PTRACE_GETREGS failed"
    );
  }

  return regs;
}
