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
  // prevent from copying
  PtraceSession& operator=(const PtraceSession&) = delete;
  // prevent from copying through = operator
  PtraceSession(PtraceSession&&) = delete;
  // prevent from moving constructor with std::move
  PtraceSession& operator=(PtraceSession&&) = delete;
  // prevent from moving constructor with std::move through = operator
  ~PtraceSession() noexcept;
  // noexcept is a specifier used to gurantee a function
  // will not throw excepctions
};
