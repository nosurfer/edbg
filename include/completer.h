#pragma once

#include <string>
#include <vector>

/**
 * @brief Инициализация readline: установка имени приложения и функции автодополнения.
 *        Должна быть вызвана один раз перед первым вызовом read_command().
 */
void init_readline();

/**
 * @brief Прочитать строку от пользователя с поддержкой редактирования и истории.
 * @param prompt Строка приглашения (например, " > ").
 * @return Указатель на строку, выделенную через malloc(), или nullptr при EOF (Ctrl+D).
 *         Пользователь обязан вызвать free() для освобождения памяти.
 */
char* read_command(const char* prompt);

/**
 * @brief Добавить строку в историю команд readline.
 * @param line Строка, которую нужно сохранить в истории (обычно непустая).
 */
void add_command_history(const char* line);

/**
 * @brief Получить список всех команд, которые понимает отладчик.
 *        Используется для автодополнения и справки.
 * @return const std::vector<std::string>&
 */
const std::vector<std::string>& get_command_list();