#pragma once

#include "ptracer.cc"
#include "command_parser.h"
#include "completer.h"

#include <cstdint>
#include <string>
#include <iostream>
#include <unordered_map>
#include <functional>

class Debugger {
private:
    Ptracer ptracer_;
    bool exit_requested_ = false;

    struct CommandInfo {
        std::string help;
        std::function<void(const ParsedCommand&)> handler;
    };
    std::unordered_map<std::string, CommandInfo> commands_;

    // Initialize the command dictionary.
    // Adding a new command: simply add one entry to this dictionary.
    // Key = command name, value = {help, lambda handler}.
    // Inside the lambda you can directly call ptracer_ methods or any other functions.
    void init_commands() {
        commands_ = {
            // Commands without arguments
            {"detach",  {"detach -- detach from process", [this](auto&){
                if (auto res = ptracer_.detach(); !res)
                    std::println(stderr, "ptrace detach: {}", res.error().message());
            }}},
            {"cont",    {"cont -- continue execution", [this](auto&){
                if (auto res = ptracer_.cont(); !res)
                    std::println(stderr, "ptrace cont: {}", res.error().message());
            }}},
            {"step",    {"step -- single step (one instruction)", [this](auto&){
                if (auto res = ptracer_.step(); !res)
                    std::println(stderr, "ptrace step: {}", res.error().message());
            }}},
            {"regs",    {"regs -- show registers", [this](auto&){
                if (auto res = ptracer_.regs(); !res)
                    std::println(stderr, "ptrace regs: {}", res.error().message());
            }}},
            {"maps",    {"maps -- show memory maps", [this](auto&){
                if (auto res = ptracer_.maps(); !res)
                    std::println(stderr, "ptrace maps: {}", res.error().message());
            }}},
            {"exit",    {"exit -- quit debugger", [this](auto&){ exit_requested_ = true; }}},
            {"disass",  {"disass -- disassemble current instruction (not ready)", [](auto&){
                std::println("disass: not implemented yet");
            }}},

            // Commands with one argument
            {"attach",  {"attach PID -- attach to running process", [this](const auto& cmd){
                if (cmd.args.empty()) {
                    std::println("Usage: attach PID");
                    return;
                }
                pid_t pid = std::stoi(cmd.args[0]);
                if (auto res = ptracer_.attach(pid); !res)
                    std::println(stderr, "ptrace attach: {}", res.error().message());
            }}},
            {"file",    {"file PATH -- spawn a new process", [this](const auto& cmd){
                if (cmd.args.empty()) {
                    std::println("Usage: file PATH");
                    return;
                }
                if (auto res = ptracer_.spawn(cmd.args[0]); !res)
                    std::println(stderr, "ptrace spawn: {}", res.error().message());
            }}},
            {"qword",   {"qword ADDR -- read 8 bytes at address", [this](const auto& cmd){
                if (cmd.args.empty()) {
                    std::println("Usage: qword ADDR");
                    return;
                }
                std::uintptr_t addr = std::stoull(cmd.args[0], nullptr, 16);
                if (auto res = ptracer_.readq(addr); !res)
                    std::println(stderr, "read: {}", res.error().message());
            }}},

            // Command 'read' with options --address and --size
            {"read",    {"read --address ADDR [--size SIZE] -- read memory (default size=8)",
                         [this](const auto& cmd){
                std::uintptr_t addr = 0;
                std::size_t size = 8;
                if (auto opt = cmd.get_option("address"))
                    addr = std::stoull(*opt, nullptr, 16);
                else if (!cmd.args.empty())
                    addr = std::stoull(cmd.args[0], nullptr, 16);
                else {
                    std::println("Usage: read --address ADDR [--size SIZE]");
                    return;
                }
                if (auto opt = cmd.get_option("size"))
                    size = std::stoull(*opt);
                if (size == 8) {
                    if (auto res = ptracer_.readq(addr); !res)
                        std::println(stderr, "read: {}", res.error().message());
                } else {
                    std::println("read: only 8-byte reads are implemented now");
                }
            }}},

            // Help
            {"help",    {"help [command] -- show this help", [this](const auto& cmd){ show_help(cmd); }}}
        };
    }

    // Display help: without arguments – list commands, with argument – details.
    void show_help(const ParsedCommand& cmd) {
        if (cmd.args.empty()) {
            std::println("Available commands:");
            for (const auto& [name, info] : commands_)
                std::println("  {}", name);
            std::println("Type 'help <command>' for details.");
        } else {
            const std::string& topic = cmd.args[0];
            auto it = commands_.find(topic);
            if (it != commands_.end())
                std::println("{}", it->second.help);
            else
                std::println("No help for '{}'", topic);
        }
    }

public:
    Debugger() {
        init_commands();
    }

    // Public method to execute a command from a string (used by main.cc and run()).
    void execute_command(const std::string& line) {
        ParsedCommand cmd = CommandParser::parse(line);
        if (cmd.command.empty()) return;

        // Map single‑character commands to full names
        static const std::unordered_map<char, std::string> short_commands = {
            {'s', "step"},
            {'c', "cont"},
            {'d', "detach"},
            {'f', "file"},
            {'x', "read"},
            {'r', "regs"},
            {'q', "exit"}
        };
        if (cmd.command.size() == 1) {
            auto it = short_commands.find(cmd.command[0]);
            if (it != short_commands.end()) {
                cmd.command = it->second;
            }
        }

        auto it = commands_.find(cmd.command);
        if (it != commands_.end())
            it->second.handler(cmd);
        else if (!cmd.command.empty())
            std::println("Unknown command: '{}'. Type 'help'.", cmd.command);
    }

    // Main REPL loop
    void run() {
        init_readline();
        char* line;
        while (!exit_requested_) {
            line = read_command(" > ");
            if (!line) break;
            add_command_history(line);
            std::string input(line);
            free(line);
            if (input.empty()) continue;

            // Delegate all command handling to execute_command()
            execute_command(input);
        }
    }
};