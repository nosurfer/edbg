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
public:
  explicit Dispatcher(pid_t pid)
    : pid_(pid) {}

  std::expected<State, std::error_code> wait(void)
  {
    int status;

    pid_t res = waitpid(pid_, &status, 0);
    if (res == -1) {
      return std::unexpected(
        std::error_code(errno, std::generic_category())
      );
    }

    if (WIFEXITED(status)) {
      state_ = State::Exited;
      exit_code_ = WEXITSTATUS(status);
      return state_;
    }
    if (WIFSIGNALED(status)) {
      state_ = State::Signaled;
      term_signal_ = WTERMSIG(status);
#ifdef WCOREDUMP
      core_dumped_ = WCOREDUMP(status);
#endif
      return state_;
    }
    if (WIFSTOPPED(status)) {
      state_ = State::Stopped;
      stop_signal_ = WSTOPSIG(status);
      return state_;
    }
    if (WIFCONTINUED(status)) {
      state_ = State::Continued;
      return state_;
    }

    state_ = State::None;
    return state_;
  }
};
