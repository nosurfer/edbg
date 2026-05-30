#pragma once

#include "ptracer.cc"
#include <string>
#include <iostream>

class Debugger {
private:
  Ptracer ptracer_;
public:
  Debugger(pid_t pid)
    : ptracer_(pid) {}
  Debugger(std::string path)
    : ptracer_(std::move(path)) {}

  void run(void) {
    switch (ptracer_.mode()) {
      case Mode::Attach: {
        attach();
        break;
      }
      case Mode::Spawn:
        spawn();
        break;
    }
    for (;;) {
      std::string input;
      std::print(" > ");
      std::getline(std::cin, input);
      if (input == "regs")
        regs();
      if (input == "exit") {
        detach();
        break;
      }
    }
  }
  void attach(void)
  {
    auto res = ptracer_.attach();
    if (!res)
      std::println(stderr, "{}", res.error().message());
  }
  void spawn(void)
  {
    auto res = ptracer_.spawn();
    if (!res)
      std::println(stderr, "{}", res.error().message());
  }
  void detach(void)
  {
    auto res = ptracer_.detach();
    if (!res)
      std::println(stderr, "{}", res.error().message());
  }
  void regs(void)
  {
    
    auto res = ptracer_.regs();
    if (!res)
      std::println(stderr, "{}", res.error().message());
  }
};
