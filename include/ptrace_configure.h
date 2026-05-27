#pragma once

#include <string>

#include <sys/types.h>
#include <sys/user.h>

void ptrace_attach(pid_t pid);
pid_t ptrace_fork(const std::string& pathname);
void ptrace_detach(pid_t pid);
struct user_regs_struct ptrace_getregs(pid_t pid);
