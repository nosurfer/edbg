#pragma once

#include <sys/types.h>

void ptrace_attach(pid_t pid);
void ptrace_detach(pid_t pid);
