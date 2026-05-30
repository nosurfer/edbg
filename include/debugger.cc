#pragma once

#include "ptracer.cc"
#include <string>
#include <iostream>

class Debugger {
private:
  Ptracer ptracer_;
public:
  Debugger() = default;
  Debugger(pid_t pid)
    : ptracer_(pid) {}
  Debugger(std::string path)
    : ptracer_(std::move(path)) {}

  void run(void) {
    switch (ptracer_.mode()) {
      case Mode::Attach:
        attach();
        break;
      case Mode::Spawn:
        spawn();
        break;
      default:
        break;
    }
    for (;;) {
      std::string input;
      std::print(" > ");
      std::getline(std::cin, input);
      if (input == "regs")
        regs();
      if (input == "detach") {
        detach();
      }
      if (input == "exit") {
        break;
      }
      if (input == "file") {
        std::print("input filepath: ");
        std::getline(std::cin, input);
        ptracer_ = Ptracer(input);
        spawn();
      }
    }
  }
  void attach(void)
  {
    auto res = ptracer_.attach();
    if (!res)
      std::println(stderr, "ptrace attach: {}", res.error().message());
  }
  void spawn(void)
  {
    auto res = ptracer_.spawn();
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
};
