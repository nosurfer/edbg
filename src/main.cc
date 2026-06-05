#include "debugger.cc"
#include <CLI/CLI.hpp>
#include <cstdlib>

int main(int argc, char** argv) {
    CLI::App app{"edbg – simple ptrace debugger"};

    int pid = 0;
    std::string file;

    app.add_option("-p,--pid", pid, "Attach to running process by PID");
    app.add_option("-f,--file", file, "Execute and debug a file");

    CLI11_PARSE(app, argc, argv);

    Debugger dbg;

    if (pid != 0) {
        dbg.execute_command("attach " + std::to_string(pid));
    } else if (!file.empty()) {
        dbg.execute_command("file " + file);
    }

    dbg.run();
    std::exit(EXIT_SUCCESS);
}