#pragma once

#include <string>
#include <sys/types.h>

void  ptrace_attach(pid_t pid);
pid_t ptrace_fork(const std::string& pathname);
void  ptrace_detach(pid_t pid);
