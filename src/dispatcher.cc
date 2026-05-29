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
  pid_t pid_;
  State state_ = State::None;

  int exit_code_ = 0;
  int term_signal_ = 0;
  int stop_signal_ = 0;
  bool core_dumped_ = false;

  void clear_state(void)
  {
    state_ = State::None;
    exit_code_ = 0;
    term_signal_ = 0;
    stop_signal_ = 0;
    core_dumped_ = false;
  }
public:
  explicit Dispatcher(pid_t pid)
    : pid_(pid) {}

  std::expected<void, std::error_code> wait(void)
  {
    int status;

    pid_t res = waitpid(pid_, &status, 0);
    if (res == -1) {
      return std::unexpected(
        std::error_code(errno, std::generic_category())
      );
    }

    clear_state();
    if (WIFEXITED(status)) {
      state_ = State::Exited;
      exit_code_ = WEXITSTATUS(status);
    }
    else if (WIFSIGNALED(status)) {
      state_ = State::Signaled;
      term_signal_ = WTERMSIG(status);
#ifdef WCOREDUMP
      core_dumped_ = WCOREDUMP(status);
#endif
    }
    else if (WIFSTOPPED(status)) {
      state_ = State::Stopped;
      stop_signal_ = WSTOPSIG(status);
    }
    else if (WIFCONTINUED(status)) {
      state_ = State::Continued;
    }

    return {};
  }
  State state(void) const noexcept { return state_; }
  bool exited(void) const noexcept { return state_ == State::Exited; }
  bool stopped(void) const noexcept { return state_ == State::Stopped; }
  bool signaled(void) const noexcept { return state_ == State::Signaled; }
  bool continued(void) const noexcept { return state_ == State::Continued; }
  std::optional<int> exit_code(void) const noexcept
  {
    if (!exited()) return std::nullopt;
    return exit_code_; 
  }
  std::optional<int> term_signal(void) const noexcept
  { 
    if (!signaled()) return std::nullopt;
    return term_signal_; 
  }
  std::optional<int> stop_signal(void) const noexcept 
  {
    if (!stopped()) return std::nullopt;
    return stop_signal_; 
  }
  std::optional<bool> core_dumped(void) const noexcept
  { 
    if (!signaled()) return std::nullopt;
    return core_dumped_; 
  }
};
