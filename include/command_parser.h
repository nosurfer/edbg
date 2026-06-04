#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <string_view>

/**
 * @brief Результат разбора одной команды.
 *
 * Пример: для строки "step 5 --verbose --count=3 -la"
 *   command = "step"
 *   options = { {"verbose",""}, {"count","3"}, {"l",""}, {"a",""} }
 *   args = { "5" }
 */
struct ParsedCommand {
    std::string command;                         // имя команды (первый токен)
    std::map<std::string, std::string> options;  // опции: ключ -> значение (пустая строка для флагов)
    std::vector<std::string> args;               // позиционные аргументы

    // Проверить наличие опции (например, has_option("verbose"))
    bool has_option(const std::string& name) const {
        return options.find(name) != options.end();
    }

    // Получить значение опции (std::nullopt, если опция отсутствует)
    std::optional<std::string> get_option(const std::string& name) const {
        auto it = options.find(name);
        if (it != options.end()) return it->second;
        return std::nullopt;
    }
};

/**
 * @brief Класс-парсер командной строки для REPL.
 *
 * Поддерживает:
 *   - Короткие опции: -a, -b, -la (разворачиваются в -l -a)
 *   - Длинные опции: --help, --count=5, --count 5
 *   - Позиционные аргументы (всё, что не начинается с '-')
 */
class CommandParser {
public:
    /**
     * @brief Разобрать строку, введённую пользователем.
     * @param line Входная строка (без перевода строки).
     * @return ParsedCommand с заполненными полями.
     */
    static ParsedCommand parse(std::string_view line);
};