#include "completer.h"
#include <readline/readline.h>   // основная библиотека readline
#include <readline/history.h>    // для истории команд
#include <cstring>               // strdup

// Список всех команд отладчика. Должен совпадать с реально поддерживаемыми командами.
static const std::vector<std::string> commands = {
    "attach", "cont", "detach", "disass", "exit", "file",
    "help", "maps", "qword", "read", "regs", "step"
};

const std::vector<std::string>& get_command_list() {
    return commands;
}

// ---- Функция-генератор для автодополнения ----
// Вызывается readline многократно для каждого возможного совпадения.
// Возвращает очередную подходящую команду (дубликат строки) или nullptr, если больше нет.
static char* command_generator(const char* text, int state) {
    static size_t idx = 0;       // текущий индекс в списке команд
    static std::string prefix;   // то, что уже набрал пользователь

    if (state == 0) {
        // Начинаем новый поиск: запоминаем префикс и сбрасываем индекс
        idx = 0;
        prefix = text;
    }

    // Ищем следующую команду, начинающуюся с prefix
    for (; idx < commands.size(); ++idx) {
        const std::string& cmd = commands[idx];
        if (cmd.compare(0, prefix.size(), prefix) == 0) {
            char* match = strdup(cmd.c_str());   // readline ожидает malloc-память
            ++idx;
            return match;
        }
    }
    return nullptr;   // больше совпадений нет
}

// ---- Функция, которую readline вызывает при нажатии Tab ----
// text – текущее слово (то, что дополняем)
// start – позиция начала слова в строке
static char** edbg_completion(const char* text, int start, int end) {
    (void)end;   // не используется
    // Дополняем только первое слово (команду). Если start > 0 – не дополняем.
    if (start == 0) {
        return rl_completion_matches(text, command_generator);
    }
    return nullptr;   // для остальных позиций дополнение не предлагаем
}

// ---- Инициализация readline ----
void init_readline() {
    rl_readline_name = "edbg";                       // имя приложения (для конфигурации)
    rl_attempted_completion_function = edbg_completion; // установка функции дополнения
    using_history();                                 // включаем историю команд
}

// ---- Чтение строки с приглашением ----
char* read_command(const char* prompt) {
    return readline(prompt);   // возвращает malloc-строку или NULL при EOF
}

// ---- Добавление строки в историю (если не пуста) ----
void add_command_history(const char* line) {
    if (line && *line) {
        add_history(line);
    }
}