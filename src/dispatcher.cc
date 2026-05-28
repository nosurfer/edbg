/*
 * man 2 wait
 * WIFEXITED - returns true if child terminated normally
 * WEXITSTATUS - returns exit status of the child
 * WIFSIGNALED - returns true if child process was terminated by a Signal
 * WTERMSIG - returns the number of signal that caused child process to terminate
 * WCOREDUMP - returns true if child process produced a core dump. Only if WIFSIGNALED returned true
 * WIFSTOPPED - returns true if the child was stopped by delivery of a signal (traced)
 * WSTOPSIG - returns number of the signal which caused the child to stop
 * WIFCONTINUED - returns true if the child process was resumed by delivery of SIGCONT
 */ 

#include <sys/wait.h>
#include <sys/types.h>

enum class WaitStatus {
  Exit,
  Stop,
  Continue
};

class Dispatcher {
private:
  pid_t pid_;
public:
  explicit Dispatcher(pid_t pid)
    : pid_(pid) {}

  int check_status(void)
  {
    int status;
    pid_t res = waitpid(pid_, &status, 0);


    // temp
    return status;
  }


};
