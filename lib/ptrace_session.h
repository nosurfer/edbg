#pragma once

#include <sys/types.h>

class PtraceSession {
private:
  pid_t pid_;
  bool attached_;
public:
  explicit PtraceSession(pid_t pid);
  // explicit gurantees that object of class is created only 
  // when you intentionaly call its constructor 
  PtraceSession(const PtraceSession&) = delete; 
  PtraceSession& operator=(const PtraceSession&) = delete;
  PtraceSession(PtraceSession&&) = delete;
  PtraceSession& operator=(PtraceSession&&) = delete;

  ~PtraceSession() noexcept;
  // noexcept is a specifier used to gurantee a function
  // will not throw excepctions
};
