#pragma once

#include "ptracer.cc"

#include <string>
#include <cstdint>
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
        pkill();
        break;
      } else if (input == "step") {
        step();
      } else if (input == "maps") {
        maps();
      } else if (input == "disass") {
        disass();
      } else if (input == "qword") {
        std::size_t size;
        std::print("address: ");
        std::getline(std::cin, input);
        std::uintptr_t addr = static_cast<std::uintptr_t>(std::stoull(input, nullptr, 16));
        std::print("count: ");
        std::cin >> size;
        readm(addr, size);
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
      std::println(stderr, "ptrace attach: {}", res.error().message());
  }
  void spawn(const std::string& pathname)
  {
    if (auto res = ptracer_.spawn(pathname); !res)
      std::println(stderr, "spawn: {}", res.error().message());
  }
  void detach(void)
  {
    if (auto res = ptracer_.detach(); !res)
      std::println(stderr, "detach: {}", res.error().message());
  }
  void pkill(void)
  {
    if (auto res = ptracer_.pkill(); !res)
      std::println(stderr, "kill: {}", res.error().message());
  }
  void regs(void)
  {
    if (auto res = ptracer_.regs(); !res)
      std::println(stderr, "regs: {}", res.error().message());
  }
  void cont(void)
  {
    if (auto res = ptracer_.cont(); !res)
      std::println(stderr, "cont: {}", res.error().message());
  }
  void step(void)
  {
    if (auto res = ptracer_.step(); !res)
      std::println(stderr, "step: {}", res.error().message());
  }
  void maps(void)
  {
    if (auto res = ptracer_.maps(); !res)
      std::println(stderr, "maps: {}", res.error().message());
  }
  void readm(std::uintptr_t address, std::size_t size)
  {
    if (auto res = ptracer_.readm(address, size); !res)
      std::println(stderr, "read: {}", res.error().message());
  }
  void disass(void)
  {
    if (auto res = ptracer_.disass(); !res)
      std::println(stderr, "disass: {}", res.error().message());
  }
};
