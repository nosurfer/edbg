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
      if (input == "regs") {
        regs();
      } else if (input == "detach") {
        detach();
      } else if (input == "cont") {
        cont();
      } else if (input == "exit") {
        break;
      } else if (input == "step") {
        step();
      } else if (input == "maps") {
        maps();
      } else if (input == "file") {
        std::print("input filepath: ");
        std::getline(std::cin, input);
        spawn(input);
      }
    }
  }
  void attach(pid_t pid)
  {
    if (auto res = ptracer_.attach(pid); !res)
      std::println(stderr, "ptrace step: {}", res.error().message());
  }
  void spawn(const std::string& pathname)
  {
    if (auto res = ptracer_.spawn(pathname); !res)
      std::println(stderr, "ptrace step: {}", res.error().message());
  }
  void detach(void)
  {
    if (auto res = ptracer_.detach(); !res)
      std::println(stderr, "ptrace step: {}", res.error().message());
  }
  void regs(void)
  {
    if (auto res = ptracer_.regs(); !res)
      std::println(stderr, "ptrace step: {}", res.error().message());
  }
  void cont(void)
  {
    if (auto res = ptracer_.cont(); !res)
      std::println(stderr, "ptrace step: {}", res.error().message());
  }
  void step(void)
  {
    if (auto res = ptracer_.step(); !res)
      std::println(stderr, "ptrace step: {}", res.error().message());
  }
  void maps(void)
  {
    if (auto res = ptracer_.maps(); !res)
      std::println(stderr, "ptrace step: {}", res.error().message());
  }
};
