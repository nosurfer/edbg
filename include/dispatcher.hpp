#pragma once
/*
 * man 2 wait
 * WIFEXITED - returns true if child terminated normally
 * WIFSTOPPED - returns true if the child was stopped by delivery of a signal (traced)
 * WIFSIGNALED - returns true if child process was terminated by a Signal
 * WIFCONTINUED - returns true if the child process was resumed by delivery of SIGCONT
 * WCOREDUMP - returns true if child process produced a core dump. Only if WIFSIGNALED returned true
 *
 * WEXITSTATUS - returns exit status of the child
 * WTERMSIG - returns the number of signal that caused child process to terminate
 * WSTOPSIG - returns number of the signal which caused the child to stop
 */ 

#include <expected>
#include <optional>
#include <system_error>

#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>

enum struct State {
  None = 0,
  Exited = 1,
  Stopped = 2,
  Signaled = 3,
  Continued = 4
};

class Dispatcher {
private:
  State state_ = State::None;

  int exit_code_ = 0;
  int term_signal_ = 0;
  int stop_signal_ = 0;
  bool core_dumped_ = false;

  void clear_state(void);
public:
  std::expected<void, std::error_code> wait(pid_t pid);
  State state(void) const noexcept;
  bool exited(void) const noexcept;
  bool stopped(void) const noexcept;
  bool signaled(void) const noexcept;
  bool continued(void) const noexcept;
  std::optional<int> exit_code(void) const noexcept;
  std::optional<int> term_signal(void) const noexcept;
  std::optional<int> stop_signal(void) const noexcept;
  std::optional<bool> core_dumped(void) const noexcept;
};
