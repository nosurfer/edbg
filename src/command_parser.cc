#include "command_parser.h"
#include <cctype>      // for isspace
#include <vector>
#include <string>

// ----- Helper function: split a string into tokens by whitespace -----
// We do not use std::stringstream because it leads to "vexing parse" on some compilers.
static std::vector<std::string> tokenize(std::string_view line) {
    std::vector<std::string> tokens;
    std::string current;   // accumulate the current token

    for (char ch : line) {
        // If a whitespace character (space, tab, newline...)
        if (std::isspace(static_cast<unsigned char>(ch))) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            // Otherwise add the character to the current token
            current.push_back(ch);
        }
    }
    // Don't forget the last token if the line does not end with whitespace
    if (!current.empty()) {
        tokens.push_back(current);
    }
    return tokens;
}

// ----- Main parsing function -----
ParsedCommand CommandParser::parse(std::string_view line) {
    ParsedCommand result;
    auto tokens = tokenize(line);
    if (tokens.empty()) return result;   // empty line

    // The first token is always the command name
    result.command = tokens[0];

    // Process the remaining tokens, starting at index 1
    for (size_t i = 1; i < tokens.size(); ++i) {
        const std::string& tok = tokens[i];

        // --- 1) Long option: starts with "--" ---
        if (tok.size() >= 3 && tok[0] == '-' && tok[1] == '-') {
            size_t eq_pos = tok.find('=');
            if (eq_pos != std::string::npos) {
                // Format: --key=value
                std::string key = tok.substr(2, eq_pos - 2);
                std::string value = tok.substr(eq_pos + 1);
                result.options[key] = value;
            } else {
                // Format: --key (possibly value in the next token)
                std::string key = tok.substr(2);
                // If the next token exists and does not start with '-', it is the value
                if (i + 1 < tokens.size() && tokens[i+1][0] != '-') {
                    result.options[key] = tokens[i+1];
                    ++i;   // skip the next token, it has been used
                } else {
                    result.options[key] = "";   // flag option without value
                }
            }
        }
        // --- 2) Short option: starts with "-" (but not "--") ---
        else if (tok.size() >= 2 && tok[0] == '-' && tok[1] != '-') {
            // Iterate over all characters after the first '-'
            for (size_t j = 1; j < tok.size(); ++j) {
                std::string key(1, tok[j]);   // single‑character key
                // If this is the last character in the group, it may take a value
                if (j == tok.size() - 1) {
                    if (i + 1 < tokens.size() && tokens[i+1][0] != '-') {
                        result.options[key] = tokens[i+1];
                        ++i;   // consume the value
                    } else {
                        result.options[key] = "";
                    }
                } else {
                    // Inside a group of options there cannot be a value – only a flag
                    result.options[key] = "";
                }
            }
        }
        // --- 3) Positional argument (does not start with '-') ---
        else {
            result.args.push_back(tok);
        }
    }

    return result;
}