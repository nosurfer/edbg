#pragma once

#include <string>
#include <vector>

/**
 * @brief Initialize readline: set application name and completion function.
 *        Must be called once before the first call to read_command().
 */
void init_readline();

/**
 * @brief Read a line from the user with editing and history support.
 * @param prompt Prompt string (e.g., " > ").
 * @return Pointer to a malloc()'ed string, or nullptr on EOF (Ctrl+D).
 *         The caller must call free() to release the memory.
 */
char* read_command(const char* prompt);

/**
 * @brief Add a line to the readline command history.
 * @param line The string to store in history (typically non‑empty).
 */
void add_command_history(const char* line);

/**
 * @brief Get the list of all commands understood by the debugger.
 *        Used for completion and help.
 * @return const std::vector<std::string>&
 */
const std::vector<std::string>& get_command_list();