#pragma once

#include "wrappers.cc"
#include "dispatcher.cc"

#include <print>
#include <utility>
#include <expected>
#include <system_error>

#include <sys/types.h>

class Ptracer {
private:
  pid_t pid_;
  bool attached_;
  Dispatcher dispatcher_;
public:
  Ptracer()
    : attached_(false) {}
  ~Ptracer() noexcept { std::ignore = detach(); }

  std::expected<void, std::error_code> attach(pid_t pid)
  {
    pid_ = pid;
    if (auto res = detach(); !res)
      return std::unexpected(res.error());
    if (auto res = ptrace_attach(pid_); !res)
      return std::unexpected(res.error());
    if (auto res = dispatcher_.wait(pid_); !res)
      return std::unexpected(res.error());
    if (!dispatcher_.stopped())
      return std::unexpected(std::make_error_code(std::errc::connection_reset));
    attached_ = true;
    return {};
  }

  std::expected<void, std::error_code> spawn(const std::string& pathname)
  {
    if (auto res = detach(); !res)
      return std::unexpected(res.error());
    auto res = ptrace_fork(pathname);
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
      return {};
    auto res = ptrace_detach(pid_);
    if (!res)
      return std::unexpected(res.error());
    attached_ = false;
    return {};
  }

  std::expected<void, std::error_code> regs(void)
  {
    if (!attached_)
      return {};
    auto regs = ptrace_getregs(pid_);
    if (!regs)
      return std::unexpected(regs.error());
    std::println("rip: {:#x}, rax: {:#x}", regs.value().rip, regs.value().rax);
    return {};
  }
};
