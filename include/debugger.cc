#pragma once

#include "ptracer.cc"

#include <string>
#include <cstdint>
#include <iostream>

class Debugger {
private:
  Ptracer ptracer_;

  std::uintptr_t parse_addr(const std::string& s)
  {
    return static_cast<std::uintptr_t>(
        std::stoull(s, nullptr, 16));
  }

public:
  void run(void)
  {
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

        std::print("address (hex): ");
        std::getline(std::cin, input);
        auto addr = parse_addr(input);

        std::size_t size{};
        std::print("count: ");
        std::cin >> size;
        std::cin.ignore();

        readm(addr, size);

      } else if (input == "file") {

        std::print("input filepath: ");
        std::getline(std::cin, input);

        spawn(input);

      } else if (input == "b") {

        std::print("breakpoint address (hex): ");
        std::getline(std::cin, input);

        auto addr = parse_addr(input);
        std::ignore = ptracer_.breakpoint(addr);

      } else if (input == "bd") {

        std::print("delete breakpoint address (hex): ");
        std::getline(std::cin, input);

        auto addr = parse_addr(input);
        std::ignore = ptracer_.breakpoint_delete(addr);
      }
    }
  }

  void attach(pid_t pid)
  {
    if (auto res = ptracer_.attach(pid); !res)
      std::println(stderr, "attach: {}", res.error().message());
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
