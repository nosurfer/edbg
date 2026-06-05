#include "completer.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <cstring>
#include <vector>
#include <algorithm>
#include <sys/ioctl.h>
#include <unistd.h>

static const std::vector<std::string> commands = {
    "attach", "cont", "detach", "disass", "exit", "file",
    "help", "maps", "qword", "read", "regs", "step", "write", "break", "delete" ,"kill"
};

const std::vector<std::string>& get_command_list() {
    return commands;
}

static void display_matches(char** matches, int /*max_len*/, int /*max_lines*/) {
    if (!matches || !*matches) return;
    printf("\n");
    for (int i = 0; matches[i]; ++i) {
        if (i > 0) printf("  ");
        printf("%s", matches[i]);
    }
    printf("\n");
    fflush(stdout);
    rl_on_new_line();
    rl_redisplay();
}

static char* command_generator(const char* text, int state) {
    static size_t idx = 0;
    static std::string prefix;

    if (state == 0) {
        idx = 0;
        prefix = text;
    }

    for (; idx < commands.size(); ++idx) {
        const std::string& cmd = commands[idx];
        if (cmd.compare(0, prefix.size(), prefix) == 0) {
            char* match = strdup(cmd.c_str());
            ++idx;
            return match;
        }
    }
    return nullptr;
}

static char** edbg_completion(const char* text, int start, int /*end*/) {
    if (start == 0)
        return rl_completion_matches(text, command_generator);
    return nullptr;
}

void init_readline() {
    rl_readline_name = "edbg";
    rl_attempted_completion_function = edbg_completion;
    rl_completion_display_matches_hook = display_matches;
    using_history();
}

char* read_command(const char* prompt) {
    return readline(prompt);
}

void add_command_history(const char* line) {
    if (line && *line)
        add_history(line);
}