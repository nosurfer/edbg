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

    void init_commands() {
        commands_ = {
            // --- Basic control commands ---
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

            // --- Disassembly (now implemented) ---
            {"disass",  {"disass [ADDR] -- disassemble code at address (or current RIP)", [this](const auto& cmd){
                std::uintptr_t addr = 0;
                if (!cmd.args.empty()) {
                    try {
                        addr = std::stoull(cmd.args[0], nullptr, 16);
                    } catch (...) {
                        std::println("Invalid address: {}", cmd.args[0]);
                        return;
                    }
                }
                if (auto res = ptracer_.disass(addr); !res)
                    std::println(stderr, "disass: {}", res.error().message());
            }}},

            // --- Process attachment / spawning ---
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

            // --- Memory read (already present) ---
            {"qword",   {"qword ADDR -- read 8 bytes at address", [this](const auto& cmd){
                if (cmd.args.empty()) {
                    std::println("Usage: qword ADDR");
                    return;
                }
                std::uintptr_t addr = std::stoull(cmd.args[0], nullptr, 16);
                if (auto res = ptracer_.readm(addr); !res)
                    std::println(stderr, "read: {}", res.error().message());
            }}},
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
                    if (auto res = ptracer_.readm(addr); !res)
                      std::println(stderr, "read: {}", res.error().message());
                } else {
                    if (auto res = ptracer_.readm(addr, size); !res)
                      std::println(stderr, "read: {}", res.error().message());
                }
            }}},

            // --- Memory write (new) ---
            {"writem",  {"writem ADDR VALUE -- write 8 bytes to memory address (both hex)", [this](const auto& cmd){
                if (cmd.args.size() < 2) {
                    std::println("Usage: writem ADDR VALUE");
                    return;
                }
                std::uintptr_t addr;
                std::uint64_t value;
                try {
                    addr = std::stoull(cmd.args[0], nullptr, 16);
                    value = std::stoull(cmd.args[1], nullptr, 16);
                } catch (...) {
                    std::println("Invalid hex argument");
                    return;
                }
                if (auto res = ptracer_.writem(addr, value); !res)
                    std::println(stderr, "writem: {}", res.error().message());
            }}},

            // --- Breakpoint management (new) ---
            {"breakpoint", {"breakpoint ADDR -- set software breakpoint", [this](const auto& cmd){
                if (cmd.args.empty()) {
                    std::println("Usage: breakpoint ADDR");
                    return;
                }
                std::uintptr_t addr;
                try {
                    addr = std::stoull(cmd.args[0], nullptr, 16);
                } catch (...) {
                    std::println("Invalid address: {}", cmd.args[0]);
                    return;
                }
                if (auto res = ptracer_.breakpoint(addr); !res)
                    std::println(stderr, "breakpoint: {}", res.error().message());
            }}},
            {"bp",      {"bp ADDR -- alias for breakpoint", [this](const auto& cmd){
                // same as breakpoint
                if (cmd.args.empty()) {
                    std::println("Usage: bp ADDR");
                    return;
                }
                std::uintptr_t addr;
                try {
                    addr = std::stoull(cmd.args[0], nullptr, 16);
                } catch (...) {
                    std::println("Invalid address: {}", cmd.args[0]);
                    return;
                }
                if (auto res = ptracer_.breakpoint(addr); !res)
                    std::println(stderr, "breakpoint: {}", res.error().message());
            }}},
            {"breakpoint-del", {"breakpoint-del ADDR -- delete breakpoint", [this](const auto& cmd){
                if (cmd.args.empty()) {
                    std::println("Usage: breakpoint-del ADDR");
                    return;
                }
                std::uintptr_t addr;
                try {
                    addr = std::stoull(cmd.args[0], nullptr, 16);
                } catch (...) {
                    std::println("Invalid address: {}", cmd.args[0]);
                    return;
                }
                if (auto res = ptracer_.breakpoint_delete(addr); !res)
                    std::println(stderr, "breakpoint-del: {}", res.error().message());
            }}},
            {"bpdel",   {"bpdel ADDR -- alias for breakpoint-del", [this](const auto& cmd){
                if (cmd.args.empty()) {
                    std::println("Usage: bpdel ADDR");
                    return;
                }
                std::uintptr_t addr;
                try {
                    addr = std::stoull(cmd.args[0], nullptr, 16);
                } catch (...) {
                    std::println("Invalid address: {}", cmd.args[0]);
                    return;
                }
                if (auto res = ptracer_.breakpoint_delete(addr); !res)
                    std::println(stderr, "bpdel: {}", res.error().message());
            }}},

            // --- Help (last) ---
            {"help",    {"help [command] -- show this help", [this](const auto& cmd){ show_help(cmd); }}}
        };
    }

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

    void execute_command(const std::string& line) {
        ParsedCommand cmd = CommandParser::parse(line);
        if (cmd.command.empty()) return;

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

            execute_command(input);
        }
        std::ignore = ptracer_.pkill();
    }
};
