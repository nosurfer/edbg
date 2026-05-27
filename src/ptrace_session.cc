#include "ptrace_configure.h"
#include "ptrace_session.h"

#include <cstdio>
#include <iostream>
#include <sys/user.h>

// constructor
PtraceSession::PtraceSession(pid_t pid)
    : pid_(pid), attached_(false)
{
  ptrace_attach(pid_);
  attached_ = true;
}

// yet another constructor
PtraceSession::PtraceSession(const std::string& pathname)
    : attached_(false)
{
  pid_ = ptrace_fork(pathname);
  attached_ = true;
}

// destructor
PtraceSession::~PtraceSession() noexcept
{
  if (attached_) {
    try {
      ptrace_detach(pid_);
    } catch (...) {
      // todo...
    }
  }
}

void PtraceSession::get_regs()
{
  if (attached_) {
    struct user_regs_struct regs = ptrace_getregs(pid_);
    std::cout << "rip: " << std::hex << regs.rip
      << " rax: " << std::hex << regs.rax << std::endl;
  }
}
