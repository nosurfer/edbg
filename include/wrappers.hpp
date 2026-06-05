#pragma once

#include <span>
#include <string>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <expected>
#include <system_error>

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>

std::expected<void, std::error_code> ptrace_attach(pid_t pid);
std::expected<pid_t, std::error_code> ptrace_fork(const std::string& pathname);
std::expected<void, std::error_code> ptrace_detach(pid_t pid);
std::expected<struct user_regs_struct, std::error_code> ptrace_getregs(pid_t pid);
std::expected<void, std::error_code> ptrace_continue(pid_t pid);
std::expected<void, std::error_code> ptrace_step(pid_t pid);
std::expected<std::ifstream, std::error_code> vmmap(pid_t pid);
std::expected<void, std::error_code>
writemem(pid_t pid, std::uintptr_t address, std::span<const std::uint64_t> data);
std::expected<void, std::error_code> ptrace_setregs(pid_t pid, const user_regs_struct& regs);

template<typename T>
std::expected<void, std::error_code>
readmem(pid_t pid, std::uintptr_t address, std::span<T> buffer)
{
  auto bytes = std::as_writable_bytes(buffer);
  constexpr std::size_t word_size = sizeof(long);

  for (std::size_t offset = 0; offset < bytes.size(); offset += word_size) {
    errno = 0;
    long word = ptrace(
      PTRACE_PEEKTEXT,
      pid,
      reinterpret_cast<void*>(address + offset),
      nullptr
    );
    if (word == -1 && errno != 0)
      return std::unexpected(std::error_code(errno, std::generic_category()));

    std::size_t copy_size = std::min(word_size, bytes.size() - offset);
    std::memcpy(bytes.data() + offset, &word, copy_size);
  }
  return {};
}
