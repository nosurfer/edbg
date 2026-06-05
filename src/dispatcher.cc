#include "dispatcher.hpp"

void Dispatcher::clear_state(void)
{
  state_ = State::None;
  exit_code_ = 0;
  term_signal_ = 0;
  stop_signal_ = 0;
  core_dumped_ = false;
}

std::expected<void, std::error_code> Dispatcher::wait(pid_t pid)
{
  int status;
  pid_t res = waitpid(pid, &status, 0);
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

State Dispatcher::state(void) const noexcept { return state_; }
bool Dispatcher::exited(void) const noexcept { return state_ == State::Exited; }
bool Dispatcher::stopped(void) const noexcept { return state_ == State::Stopped; }
bool Dispatcher::signaled(void) const noexcept { return state_ == State::Signaled; }
bool Dispatcher::continued(void) const noexcept { return state_ == State::Continued; }

std::optional<int> Dispatcher::exit_code(void) const noexcept
{
  if (!exited()) return std::nullopt;
  return exit_code_; 
}
std::optional<int> Dispatcher::term_signal(void) const noexcept
{ 
  if (!signaled()) return std::nullopt;
  return term_signal_; 
}
std::optional<int> Dispatcher::stop_signal(void) const noexcept 
{
  if (!stopped()) return std::nullopt;
  return stop_signal_; 
}
std::optional<bool> Dispatcher::core_dumped(void) const noexcept
{ 
  if (!signaled()) return std::nullopt;
  return core_dumped_; 
}
