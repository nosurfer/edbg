#pragma once

#include "wrappers.hpp"
#include "dispatcher.hpp"
#include "disassembler.hpp"

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

  std::expected<void, std::error_code> wait_status(void);
  std::expected<void, std::error_code> handle_breakpoint(std::uintptr_t rip);
public:
  Ptracer() : attached_(false) {}
  ~Ptracer() noexcept { std::ignore = detach(); }

  std::expected<void, std::error_code> attach(pid_t pid);
  std::expected<void, std::error_code> spawn(const std::string& pathname);
  std::expected<void, std::error_code> detach();
  std::expected<void, std::error_code> regs(void);
  std::expected<void, std::error_code> cont(void);
  std::expected<void, std::error_code> step(void);
  std::expected<void, std::error_code> maps(void);

  std::expected<void, std::error_code>
  readm(std::uintptr_t address, std::size_t size = 1);

  std::expected<void, std::error_code>
  writem(std::uintptr_t address, std::uint64_t value);

  std::expected<void, std::error_code>
  breakpoint(std::uintptr_t address);

  std::expected<void, std::error_code>
  breakpoint_delete(std::uintptr_t address);

  std::expected<void, std::error_code> pkill(void);

  std::expected<void, std::error_code>
  disass(std::uintptr_t address = 0, std::size_t size = 128);
};
