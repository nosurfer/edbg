#pragma once

#include "wrappers.cc"
#include "dispatcher.cc"

#include <print>
#include <utility>
#include <expected>
#include <iostream>
#include <system_error>

#include <sys/types.h>

class Ptracer {
private:
  pid_t pid_;
  bool attached_;
  Dispatcher dispatcher_;
  std::expected<void, std::error_code> wait_status(void)
  {
    if (auto res = dispatcher_.wait(pid_); !res)
      return std::unexpected(res.error());
    switch (dispatcher_.state()) {
      case State::Exited:
        std::println(stdout, "process exited with status: {}", dispatcher_.exit_code().value());
        attached_ = false;
        break;
      case State::Signaled:
#ifdef WCOREDUMP
        if (dispatcher_.core_dumped().value_or(false))
          std::println(stdout, "process core dumped");
#endif
        std::println(stdout, "process exited with signal: {}", dispatcher_.term_signal().value());
        attached_ = false;
        break;
      case State::Stopped:
        std::println(stdout, "process stopped with signal: {}", dispatcher_.stop_signal().value());
        attached_ = true;
        break;
      case State::Continued:
        std::println("process continued");
        attached_ = true;
        break;
      case State::None:
        return std::unexpected(
            std::make_error_code(std::errc::state_not_recoverable)
        );
    }
    return {};
  }
public:
  Ptracer()
    : attached_(false) {}
  ~Ptracer() noexcept { std::ignore = detach(); }

  std::expected<void, std::error_code> attach(pid_t pid)
  {
    pid_ = pid;
    std::ignore = detach();
    if (auto res = ptrace_attach(pid_); !res)
      return std::unexpected(res.error());
    return wait_status();
  }

  std::expected<void, std::error_code> spawn(const std::string& pathname)
  {
    std::ignore = detach();
    auto pid = ptrace_fork(pathname);
    if (!pid)
      return std::unexpected(pid.error());
    pid_ = *pid;
    std::println(stdout, "ptrace: spawned process with pid {}", pid_);
    return wait_status();
  }

  std::expected<void, std::error_code> detach()
  {
    if (!attached_)
      return {};
    if (auto res = ptrace_detach(pid_); !res)
      return std::unexpected(res.error());
    attached_ = false;
    std::println("ptrace: process detached");
    return {};
  }

  std::expected<void, std::error_code> regs(void)
  {
    if (!attached_) {
      std::println("registers: attach to process");
      return {};
    }
    auto regs = ptrace_getregs(pid_);
    if (!regs)
      return std::unexpected(regs.error());
    std::println("rip: {:#x}, rax: {:#x}", regs.value().rip, regs.value().rax);
    return {};
  }

  std::expected<void, std::error_code> cont(void)
  {
    if (!attached_) {
      std::println("continue: attach to process");
      return {};
    }
    if (auto res = ptrace_continue(pid_); !res)
      return std::unexpected(res.error());
    return wait_status();
  }

  std::expected<void, std::error_code> step(void)
  {
    if (!attached_) {
      std::println("step: attach to process");
      return {};
    }
    if (auto res = ptrace_step(pid_); !res)
      return std::unexpected(res.error());
    return wait_status();
  }
  
  std::expected<void, std::error_code> maps(void)
  {
    if (!attached_) {
      std::println("maps: attach to process");
      return {};
    }
    auto maps = vmmap(pid_);
    if (!maps)
      return std::unexpected(maps.error());
    std::cout << maps->rdbuf();
    return {};
  }
};
