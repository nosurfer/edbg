#pragma once

#include "ptracer.cc"

#include <string>
#include <iostream>

class Debugger {
private:
  Ptracer ptracer_;
public:
  void run(void) {
    for (;;) {
      std::string input;
      std::print(" > ");
      std::getline(std::cin, input);
      if (input == "regs")
        regs();
      else if (input == "detach") {
        detach();
      }
      else if (input == "cont") {
        cont();
      }
      else if (input == "exit") {
        break;
      }
      else if (input == "file") {
        std::print("input filepath: ");
        std::getline(std::cin, input);
        spawn(input);
      }
    }
  }
  void attach(pid_t pid)
  {
    auto res = ptracer_.attach(pid);
    if (!res)
      std::println(stderr, "ptrace attach: {}", res.error().message());
  }
  void spawn(const std::string& pathname)
  {
    auto res = ptracer_.spawn(pathname);
    if (!res)
      std::println(stderr, "ptrace spawn: {}", res.error().message());
  }
  void detach(void)
  {
    auto res = ptracer_.detach();
    if (!res)
      std::println(stderr, "ptrace detach: {}", res.error().message());
  }
  void regs(void)
  {
    auto res = ptracer_.regs();
    if (!res)
      std::println(stderr, "ptrace get regs: {}", res.error().message());
  }
  void cont(void)
  {
    auto res = ptracer_.cont();
    if (!res)
      std::println(stderr, "ptrace continued: {}", res.error().message());
  }
};
