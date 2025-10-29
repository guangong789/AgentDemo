#include "agent.h"
#include <iostream>
#include <csignal>
#include <atomic>
#include <readline/readline.h>
#include <readline/history.h>

using namespace std;
atomic<bool> stop_stream{false};

void handle_sigint(int) {
    stop_stream = true;
}

int main() {
    signal(SIGINT, handle_sigint);

    Agent agent;
    cout << "🤖 DeepSeek Chat, 有什么可以帮你? 💬" << endl;

    while (true) {
        char* input_cstr = readline("👤 User: ");
        if (!input_cstr) {  // ctrl+D 或 EOF
            cout << endl;
            break;
        }

        string input(input_cstr);
        free(input_cstr);

        if (input.empty()) continue;

        add_history(input.c_str());
        if (input == "exit") break;

        stop_stream = false;
        cout << "🤖 Assistant: " << flush;
        agent.send_msg(input, [](const string& chunk) {
            if (stop_stream.load()) return;
            cout << chunk << flush;
        });
        cout << endl;
    }
    return 0;
}