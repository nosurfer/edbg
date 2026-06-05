#pragma once

#include "wrappers.cc"
#include "disassembler.cc"
#include "dispatcher.cc"

#include <print>
#include <string>
#include <vector>
#include <csignal>
#include <cstdint>
#include <cstddef>
#include <utility>
#include <expected>
#include <iostream>
#include <system_error>
#include <unordered_map>

#include <sys/types.h>

class Ptracer {
private:
  pid_t pid_;
  bool attached_;
  Dispatcher dispatcher_;

  struct Breakpoint {
    std::uintptr_t address;
    std::uint8_t original_byte;
    bool enabled;
  };

  std::unordered_map<std::uintptr_t, Breakpoint> breakpoints_;

  std::expected<void, std::error_code> wait_status(void)
  {
    if (auto res = dispatcher_.wait(pid_); !res)
      return std::unexpected(res.error());

    switch (dispatcher_.state()) {

      case State::Exited:
        std::println(stdout, "process exited with status: {}",
                     dispatcher_.exit_code().value());
        attached_ = false;
        break;

      case State::Signaled:
#ifdef WCOREDUMP
        if (dispatcher_.core_dumped().value_or(false))
          std::println(stdout, "process core dumped");
#endif
        std::println(stdout, "process exited with signal: {}",
                     dispatcher_.term_signal().value());
        attached_ = false;
        break;

      case State::Stopped:
        std::println(stdout, "process stopped with signal: {}",
                     dispatcher_.stop_signal().value());
        attached_ = true;

        if (dispatcher_.stop_signal() == SIGTRAP) {
          auto regs = ptrace_getregs(pid_);
          if (regs) {
            auto rip = regs->rip - 1;

            if (breakpoints_.contains(rip)) {
              std::println(stdout, "breakpoint hit at {:#x}", rip);
            }
          }
        }
        break;

      case State::Continued:
        std::println("process continued");
        attached_ = true;
        break;

      case State::None:
        return std::unexpected(
            std::make_error_code(std::errc::state_not_recoverable));
    }

    return {};
  }

public:
  Ptracer() : attached_(false) {}
  ~Ptracer() noexcept { std::ignore = detach(); }

  std::expected<void, std::error_code> attach(pid_t pid)
  {
    std::ignore = detach();
    pid_ = pid;
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

    const auto& r = regs.value();

    std::println("rax: {:#x}", r.rax);
    std::println("rbx: {:#x}", r.rbx);
    std::println("rcx: {:#x}", r.rcx);
    std::println("rdx: {:#x}", r.rdx);
    std::println("rsi: {:#x}", r.rsi);
    std::println("rdi: {:#x}", r.rdi);
    std::println("rbp: {:#x}", r.rbp);
    std::println("rsp: {:#x}", r.rsp);
    std::println("r8:  {:#x}", r.r8);
    std::println("r9:  {:#x}", r.r9);
    std::println("r10: {:#x}", r.r10);
    std::println("r11: {:#x}", r.r11);
    std::println("r12: {:#x}", r.r12);
    std::println("r13: {:#x}", r.r13);
    std::println("r14: {:#x}", r.r14);
    std::println("r15: {:#x}", r.r15);
    std::println("rip: {:#x}", r.rip);
    std::println("eflags: {:#x}", r.eflags);
    std::println("orig_rax: {:#x}", r.orig_rax);
    std::println("fs_base: {:#x}", r.fs_base);
    std::println("gs_base: {:#x}", r.gs_base);

    return {};
  }

  std::expected<void, std::error_code> cont(void)
  {
    if (!attached_) {
      std::println("continue: attach to process");
      return {};
    }
    auto regs = ptrace_getregs(pid_);
    if (!regs)
      return std::unexpected(regs.error());

    if (regs) {
      auto rip = regs->rip - 1;

      if (breakpoints_.contains(rip)) {
        auto& bp = breakpoints_[rip];
        std::uint64_t word{};
        if (auto res = readmem(pid_, rip, std::span{&word, 1}); !res)
          return std::unexpected(res.error());

        word &= ~0xffULL;
        word |= bp.original_byte;

        if (auto res = writemem(pid_, rip, std::span{&word, 1}); !res)
          return std::unexpected(res.error());

        regs->rip -= 1;
        if (auto res = ptrace_setregs(pid_, *regs); !res)
          return std::unexpected(res.error());

        if (auto res = ptrace_step(pid_); !res)
          return std::unexpected(res.error());
        if (auto res = wait_status(); !res)
          return std::unexpected(res.error());

        word &= ~0xffULL;
        word |= 0xCCULL;
        if (auto res = writemem(pid_, rip, std::span{&word, 1}); !res)
          return std::unexpected(res.error());
      }
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

  std::expected<void, std::error_code>
  readm(std::uintptr_t address, std::size_t size = 1)
  {
    if (!attached_) {
      std::println("readm: attach to process");
      return {};
    }
    std::vector<std::uint64_t> values(size);
    if (auto result = readmem(pid_, address, std::span{values}); !result)
      return std::unexpected(result.error());

    for (std::size_t i = 0; i < values.size(); i++) {
      std::print("{:#018x}: {:016x}\n",
                 address + i * 8,
                 values[i]);
    }
    return {};
  }

  std::expected<void, std::error_code>
  writem(std::uintptr_t address, std::uint64_t value)
  {
    if (!attached_) {
      std::println("writem: attach to process");
      return {};
    }
    if (auto result = writemem(pid_, address, std::span{&value, 1}); !result)
      return std::unexpected(result.error());
    std::println("writem: ok");
    return {};
  }

  std::expected<void, std::error_code>
  breakpoint(std::uintptr_t address)
  {
    if (!attached_) {
      std::println("breakpoint: attach to process");
      return {};
    }
    if (breakpoints_.contains(address)) {
      std::println("breakpoint: already exists");
      return {};
    }
    std::uint64_t word{};
    if (auto res = readmem(pid_, address, std::span{&word, 1}); !res)
      return std::unexpected(res.error());

    Breakpoint bp{
      .address = address,
      .original_byte = static_cast<std::uint8_t>(word & 0xff),
      .enabled = true
    };

    word &= ~0xffULL;
    word |= 0xCCULL;

    if (auto res = writemem(pid_, address, std::span{&word, 1}); !res)
      return std::unexpected(res.error());

    breakpoints_[address] = bp;

    std::println("breakpoint set at {:#x}", address);
    return {};
  }

  std::expected<void, std::error_code>
  breakpoint_delete(std::uintptr_t address)
  {
    if (!attached_) {
      std::println("pkill: attach to process");
      return {};
    }
    auto it = breakpoints_.find(address);
    if (it == breakpoints_.end()) {
      std::println("breakpoint not found");
      return {};
    }

    std::uint64_t word{};
    if (auto res = readmem(pid_, address, std::span{&word, 1}); !res)
      return std::unexpected(res.error());

    word &= ~0xffULL;
    word |= it->second.original_byte;

    if (auto res = writemem(pid_, address, std::span{&word, 1}); !res)
      return std::unexpected(res.error());

    breakpoints_.erase(it);

    std::println("breakpoint removed at {:#x}", address);
    return {};
  }

  std::expected<void, std::error_code> pkill(void)
  {
    if (!attached_) {
      std::println("pkill: attach to process");
      return {};
    }
    if (auto res = kill(pid_, SIGKILL); res == -1)
      return std::unexpected(std::error_code(errno, std::generic_category()));
    std::println("kill: process killed");
    return {};
  }

  std::expected<void, std::error_code>
  disass(std::uintptr_t address = 0, std::size_t size = 128)
  {
    if (!attached_) {
      std::println("disass: attach to process");
      return {};
    }
    if (address == 0) {
      auto regs = ptrace_getregs(pid_);
      if (!regs)
        return std::unexpected(regs.error());
      address = regs->rip;
    }
    std::vector<std::uint8_t> code(size);
    if (auto res = readmem(pid_, address, std::span{code}); !res)
      return std::unexpected(res.error());
    return disassembly(std::span{code}, address);
  }
};
