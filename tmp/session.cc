#pragma once

#include "dispatcher.cc"

#include <sys/types.h>

class Session : public Dispatcher {
private:
  pid_t pid_;
  bool attached_;
public:
  explicit Session(pid_t pid)
    : pid_(pid), attached_(false) {}
  std::expected<void, std::error_code> wait()
  {
    return Dispatcher::wait(pid_);
  }
  pid_t pid(void) const noexcept { return pid_; }
  bool attached(void) const noexcept { return attached_; }
  void mark_attached(void) noexcept { attached_ = true; }
  void mark_detached(void) noexcept { attached_ = false; }
};
