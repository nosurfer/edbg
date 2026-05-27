#include "ptrace_configure.h"
#include "ptrace_session.h"

// constructor
PtraceSession::PtraceSession(pid_t pid)
    : pid_(pid), attached_(false)
{
  ptrace_attach(pid_);
  attached_ = true;
}

PtraceSession::PtraceSession(const std::string& pathname)
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
