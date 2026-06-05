#pragma once

#include "ptracer.hpp"
#include "completer.hpp"
#include "command_parser.hpp"

#include <unordered_map>
#include <functional>
#include <cstdint>
#include <string>

class Debugger {
private:
  Ptracer ptracer_;
  bool exit_requested_ = false;

  struct CommandInfo {
    std::string help;
    std::function<void(const ParsedCommand&)> handler;
  };
  std::unordered_map<std::string, CommandInfo> commands_;

  void init_commands();
  void show_help(const ParsedCommand& cmd);
public:
  Debugger() { init_commands(); }
  void execute_command(const std::string& line);
  void run();
};
