#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <string_view>

/**
 * @brief Result of parsing a single command.
 *
 * Example: for string "step 5 --verbose --count=3 -la"
 *   command = "step"
 *   options = { {"verbose",""}, {"count","3"}, {"l",""}, {"a",""} }
 *   args = { "5" }
 */
struct ParsedCommand {
    std::string command;                         // command name (first token)
    std::map<std::string, std::string> options;  // options: key -> value (empty string for flags)
    std::vector<std::string> args;               // positional arguments

    // Check if an option exists (e.g., has_option("verbose"))
    bool has_option(const std::string& name) const {
        return options.find(name) != options.end();
    }

    // Get option value (std::nullopt if option not present)
    std::optional<std::string> get_option(const std::string& name) const {
        auto it = options.find(name);
        if (it != options.end()) return it->second;
        return std::nullopt;
    }
};

/**
 * @brief Command line parser class for REPL.
 *
 * Supports:
 *   - Short options: -a, -b, -la (expanded to -l -a)
 *   - Long options: --help, --count=5, --count 5
 *   - Positional arguments (anything not starting with '-')
 */
class CommandParser {
public:
    /**
     * @brief Parse a user‑entered string.
     * @param line Input string (without newline).
     * @return ParsedCommand with filled fields.
     */
    static ParsedCommand parse(std::string_view line);
};