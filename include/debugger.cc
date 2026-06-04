#pragma once

#include "ptracer.cc"
#include "command_parser.h"
#include "completer.h"

#include <cstdint>
#include <string>
#include <iostream>
#include <sstream>

class Debugger {
private:
    Ptracer ptracer_;

    // ========== НОВЫЕ ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ (только они и run() изменяются) ==========

    /// Вывод справки по командам.
    void show_help(const ParsedCommand& cmd) {
        if (cmd.args.empty()) {
            // Справка общего вида: список команд
            std::println("Available commands:");
            for (const auto& c : get_command_list()) {
                std::println("  {}", c);
            }
            std::println("Type 'help <command>' for details.");
        } else {
            // Справка по конкретной команде
            const std::string& topic = cmd.args[0];
            if (topic == "step") {
                std::println("step -- single step (one instruction)");
            } else if (topic == "read") {
                std::println("read --address ADDR [--size SIZE] -- read memory (default size=8)");
            } else if (topic == "file") {
                std::println("file PATH -- spawn a new process");
            } else if (topic == "regs") {
                std::println("regs -- show registers");
            } else if (topic == "cont") {
                std::println("cont -- continue execution");
            } else if (topic == "detach") {
                std::println("detach -- detach from process");
            } else if (topic == "maps") {
                std::println("maps -- show memory maps");
            } else if (topic == "qword") {
                std::println("qword ADDR -- read 8 bytes at address");
            } else if (topic == "exit") {
                std::println("exit -- quit debugger");
            } else if (topic == "attach") {
                std::println("attach PID -- attach to running process");
            } else {
                std::println("No help for '{}'", topic);
            }
        }
    }

    /// Обработка команды "read" (читает память, используя существующий readq).
    void handle_read(const ParsedCommand& cmd) {
        std::uintptr_t addr = 0;
        std::size_t size = 8;   // по умолчанию 8 байт (qword)
        // Пытаемся получить адрес из опции --address или из первого позиционного аргумента
        if (auto opt = cmd.get_option("address")) {
            addr = std::stoull(*opt, nullptr, 16);
        } else if (!cmd.args.empty()) {
            addr = std::stoull(cmd.args[0], nullptr, 16);
        } else {
            std::println("Usage: read --address ADDR [--size SIZE]");
            return;
        }
        // Получить размер (опционально)
        if (auto opt = cmd.get_option("size")) {
            size = std::stoull(*opt);
        }
        // Пока поддерживаем только чтение 8 байт (можно расширить позже)
        if (size == 8) {
            readq(addr);   // вызов существующего метода
        } else {
            std::println("read: only 8-byte reads are implemented now");
        }
    }

public:
    // ========== ИЗМЕНЁННЫЙ МЕТОД run() – теперь использует readline и парсер ==========
    void run(void) {
        init_readline();          // инициализация автодополнения и истории
        char* line;
        for (;;) {
            line = read_command(" > ");   // получить строку с приглашением
            if (!line) break;             // Ctrl+D – выход

            add_command_history(line);    // сохранить в историю, если не пусто

            std::string input(line);
            free(line);                   // освободить память, выделенную readline
            if (input.empty()) continue;  // пустая строка – ничего не делаем

            // Разобрать команду с помощью нашего парсера
            ParsedCommand cmd = CommandParser::parse(input);

            // Глобальная поддержка --help для любой команды
            if (cmd.has_option("help")) {
                show_help(cmd);
                continue;
            }

            // ===== ДИСПЕТЧЕР КОМАНД =====
            // Все старые команды вызываются без изменений.
            // Добавлены новые: help, read, attach (раньше attach не был доступен в REPL).
            if (cmd.command == "regs") {
                regs();                 // старый метод
            } else if (cmd.command == "detach") {
                detach();               // старый
            } else if (cmd.command == "cont") {
                cont();                 // старый
            } else if (cmd.command == "exit") {
                break;
            } else if (cmd.command == "step") {
                step();                 // старый (один шаг)
            } else if (cmd.command == "maps") {
                maps();                 // старый
            } else if (cmd.command == "qword") {
                if (!cmd.args.empty()) {
                    std::uintptr_t addr = std::stoull(cmd.args[0], nullptr, 16);
                    readq(addr);        // старый readq
                } else {
                    std::println("qword ADDR");
                }
            } else if (cmd.command == "file") {
                if (!cmd.args.empty()) {
                    spawn(cmd.args[0]); // старый spawn
                } else {
                    std::println("file PATH");
                }
            } else if (cmd.command == "help") {
                show_help(cmd);         // новая команда
            } else if (cmd.command == "read") {
                handle_read(cmd);       // новая команда
            } else if (cmd.command == "attach") {
                if (!cmd.args.empty()) {
                    pid_t pid = std::stoi(cmd.args[0]);
                    attach(pid);        // старый attach (раньше не был доступен)
                } else {
                    std::println("attach PID");
                }
            } else if (!cmd.command.empty()) {
                std::println("Unknown command: '{}'. Type 'help'.", cmd.command);
            }
        }
    }

    // ========== СТАРЫЕ МЕТОДЫ (НЕ ИЗМЕНЯЮТСЯ) – приведены для полноты ==========
    void attach(pid_t pid) {
        if (auto res = ptracer_.attach(pid); !res)
            std::println(stderr, "ptrace attach: {}", res.error().message());
    }
    void spawn(const std::string& pathname) {
        if (auto res = ptracer_.spawn(pathname); !res)
            std::println(stderr, "ptrace spawn: {}", res.error().message());
    }
    void detach(void) {
        if (auto res = ptracer_.detach(); !res)
            std::println(stderr, "ptrace detach: {}", res.error().message());
    }
    void regs(void) {
        if (auto res = ptracer_.regs(); !res)
            std::println(stderr, "ptrace regs: {}", res.error().message());
    }
    void cont(void) {
        if (auto res = ptracer_.cont(); !res)
            std::println(stderr, "ptrace cont: {}", res.error().message());
    }
    void step(void) {
        if (auto res = ptracer_.step(); !res)
            std::println(stderr, "ptrace step: {}", res.error().message());
    }
    void maps(void) {
        if (auto res = ptracer_.maps(); !res)
            std::println(stderr, "ptrace maps: {}", res.error().message());
    }
    void readq(std::uintptr_t address) {
        if (auto res = ptracer_.readq(address); !res)
            std::println(stderr, "read: {}", res.error().message());
    }
};