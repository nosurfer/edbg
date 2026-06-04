#pragma once

#include <cerrno>
#include <string>
#include <cstdlib>
#include <fstream>
#include <expected>
#include <iostream>
#include <system_error>

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>

#include <stdlib.h>
#include <unistd.h>

std::expected<void, std::error_code> ptrace_attach(pid_t pid)
{
    long res = ptrace(PTRACE_ATTACH, pid, nullptr, nullptr);
    if (res == -1)
      return std::unexpected(std::error_code(errno, std::generic_category()));
    return {};
}

std::expected<pid_t, std::error_code> ptrace_fork(const std::string& pathname)
{
  // If the current program is being ptraced, a SIGTRAP
  // signal is sent to it after a successful execve().
  pid_t pid = fork();
  if (pid == -1)
    return std::unexpected(std::error_code(errno, std::generic_category()));
  if (pid == 0) {
    ptrace(PTRACE_TRACEME, 0, nullptr, nullptr);
    execl(pathname.c_str(), pathname.c_str(), (char*)nullptr);
    std::_Exit(-1);
  }
  return pid;
}

std::expected<void, std::error_code> ptrace_detach(pid_t pid)
{
  long res = ptrace(PTRACE_DETACH, pid, nullptr, nullptr);
  if (res == -1)
    return std::unexpected(std::error_code(errno, std::generic_category()));
  return {};
}

std::expected<struct user_regs_struct, std::error_code> ptrace_getregs(pid_t pid)
{
  struct user_regs_struct regs;
  long res = ptrace(PTRACE_GETREGS, pid, nullptr, &regs);
  if (res == -1)
    return std::unexpected(std::error_code(errno, std::generic_category()));
  return regs;
}

std::expected<void, std::error_code> ptrace_continue(pid_t pid)
{
  long res = ptrace(PTRACE_CONT, pid, nullptr, nullptr);
  if (res == -1)
    return std::unexpected(std::error_code(errno, std::generic_category()));
  return {};
}

std::expected<void, std::error_code> ptrace_step(pid_t pid)
{
  long res = ptrace(PTRACE_SINGLESTEP, pid, nullptr, nullptr);
  if (res == -1)
    return std::unexpected(std::error_code(errno, std::generic_category()));
  return {};
}

std::expected<std::ifstream, std::error_code> vmmap(pid_t pid)
{
   std::ifstream maps("/proc/" + std::to_string(pid) + "/maps");
   if (!maps)
     return std::unexpected(std::error_code(errno, std::generic_category()));
   return maps;
}
