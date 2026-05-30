#pragma once

#include "wrappers.cc"
#include "dispatcher.cc"

#include <print>
#include <expected>
#include <system_error>

#include <sys/types.h>

enum struct Mode {
  Attach,
  Spawn
};

class Ptracer {
private:
  pid_t pid_;
  bool attached_;
  std::string pathname_;
  Dispatcher dispatcher_;
  Mode mode_;
public:
  explicit Ptracer(pid_t pid)
    : pid_(pid), attached_(false), mode_(Mode::Attach) {}
  explicit Ptracer(std::string pathname)
    : attached_(false), pathname_(std::move(pathname)), mode_(Mode::Spawn) {}

  std::expected<void, std::error_code> attach(void)
  {
    if (mode_ != Mode::Attach)
      return std::unexpected(std::make_error_code(std::errc::operation_not_permitted));
    if (auto res = ptrace_attach(pid_); !res)
      return std::unexpected(res.error());
    if (auto res = dispatcher_.wait(pid_); !res)
      return std::unexpected(res.error());
    if (!dispatcher_.stopped())
      return std::unexpected(std::make_error_code(std::errc::connection_reset));
    attached_ = true;
    return {};
  }

  std::expected<void, std::error_code> spawn()
  {
    if (mode_ != Mode::Spawn)
      return std::unexpected(std::make_error_code(std::errc::operation_not_permitted));
    auto res = ptrace_fork(pathname_);
    if (!res)
      return std::unexpected(res.error());
    pid_ = res.value();
    if (auto res = dispatcher_.wait(pid_); !res)
      return std::unexpected(res.error());
    if (!dispatcher_.stopped())
      return std::unexpected(std::make_error_code(std::errc::connection_reset));
    attached_ = true;
    return {};
  }

  std::expected<void, std::error_code> detach()
  {
    if (!attached_)
      return std::unexpected(std::make_error_code(std::errc::operation_not_permitted));
    auto res = ptrace_detach(pid_);
    if (!res)
      return std::unexpected(res.error());
    attached_ = false;
    return {};
  }

  std::expected<void, std::error_code> regs(void)
  {
    if (!attached_)
      return std::unexpected(std::make_error_code(std::errc::operation_not_permitted));
    auto regs = ptrace_getregs(pid_);
    if (!regs)
      return std::unexpected(regs.error());
    std::println("rip: {:#x}, rax: {:#x}", regs.value().rip, regs.value().rax);
    return {};
  }

  pid_t pid(void) const noexcept { return pid_; }
  Mode mode(void) const noexcept { return mode_; }
  bool attached(void) const noexcept { return attached_; }
};
