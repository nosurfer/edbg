#include "command_parser.h"
#include <cctype>      // для isspace
#include <vector>
#include <string>

// ----- Вспомогательная функция: разбиение строки на токены по пробельным символам -----
// Мы не используем std::stringstream, так как он приводит к "vexing parse" на некоторых компиляторах.
static std::vector<std::string> tokenize(std::string_view line) {
    std::vector<std::string> tokens;
    std::string current;   // накапливаем текущий токен

    for (char ch : line) {
        // Если встретили пробельный символ (пробел, табуляция, перевод строки...)
        if (std::isspace(static_cast<unsigned char>(ch))) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            // Иначе добавляем символ к текущему токену
            current.push_back(ch);
        }
    }
    // Не забываем последний токен, если строка не заканчивается пробелом
    if (!current.empty()) {
        tokens.push_back(current);
    }
    return tokens;
}

// ----- Основная функция разбора -----
ParsedCommand CommandParser::parse(std::string_view line) {
    ParsedCommand result;
    auto tokens = tokenize(line);
    if (tokens.empty()) return result;   // пустая строка

    // Первый токен всегда имя команды
    result.command = tokens[0];

    // Обрабатываем остальные токены, начиная с индекса 1
    for (size_t i = 1; i < tokens.size(); ++i) {
        const std::string& tok = tokens[i];

        // --- 1) Длинная опция: начинается с "--" ---
        if (tok.size() >= 3 && tok[0] == '-' && tok[1] == '-') {
            size_t eq_pos = tok.find('=');
            if (eq_pos != std::string::npos) {
                // Формат: --key=value
                std::string key = tok.substr(2, eq_pos - 2);
                std::string value = tok.substr(eq_pos + 1);
                result.options[key] = value;
            } else {
                // Формат: --key (возможно, значение в следующем токене)
                std::string key = tok.substr(2);
                // Если следующий токен существует и не начинается с '-', это значение
                if (i + 1 < tokens.size() && tokens[i+1][0] != '-') {
                    result.options[key] = tokens[i+1];
                    ++i;   // пропускаем следующий токен, он уже использован
                } else {
                    result.options[key] = "";   // опция-флаг без значения
                }
            }
        }
        // --- 2) Короткая опция: начинается с "-" (но не "--") ---
        else if (tok.size() >= 2 && tok[0] == '-' && tok[1] != '-') {
            // Перебираем все символы после первого '-'
            for (size_t j = 1; j < tok.size(); ++j) {
                std::string key(1, tok[j]);   // ключ из одного символа
                // Если это последний символ в группе, он может захватить значение
                if (j == tok.size() - 1) {
                    if (i + 1 < tokens.size() && tokens[i+1][0] != '-') {
                        result.options[key] = tokens[i+1];
                        ++i;   // съедаем значение
                    } else {
                        result.options[key] = "";
                    }
                } else {
                    // Внутри группы опций значения быть не может – только флаг
                    result.options[key] = "";
                }
            }
        }
        // --- 3) Позиционный аргумент (не начинается с '-') ---
        else {
            result.args.push_back(tok);
        }
    }

    return result;
}