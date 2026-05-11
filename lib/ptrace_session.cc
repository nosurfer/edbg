#include "ptrace_configure.h"
#include "ptrace_session.h"

PtraceSession::PtraceSession(pid_t pid)
    : pid_(pid), attached_(false)
{
  ptrace_attach(pid_);
  attached_ = true;
}

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
