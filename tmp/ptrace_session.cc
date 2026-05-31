// [!] temp include
#include "dispatcher.cc"

#include "ptrace_configure.cc"
#include "ptrace_session.h"

#include <string>
#include <print>

#include <sys/user.h>

// constructor
PtraceSession::PtraceSession(pid_t pid)
    : pid_(pid), attached_(false), dispatcher_(pid_)

{
  ptrace_attach(pid_);
  auto res = dispatcher_.wait();

  if (!res) {
    std::println(stderr, "wait failed: {}", res.error().message());
    return;
  }

  if (dispatcher_.stopped())
    attached_ = true;
  else
    std::println(stderr, "failed attach to the process");
}

// yet another constructor
PtraceSession::PtraceSession(const std::string& pathname)
    : pid_(ptrace_fork(pathname)), attached_(false), dispatcher_(pid_)
{
  auto res = dispatcher_.wait();
  if (!res) {
    std::println(stderr, "wait failed: {}", res.error().message());
    return;
  }
  if (dispatcher_.stopped())
    attached_ = true;
  else
    std::println(stderr, "failed attach to the process");
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

void PtraceSession::regs()
{
  if (attached_) {
    struct user_regs_struct regs = ptrace_getregs(pid_);
    std::println("rip: {:#x}, rax: {:#x}", regs.rip, regs.rax);
  }
}
