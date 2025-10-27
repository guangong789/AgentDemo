#include "agent.h"
#include "logger_init.h"
#include <iostream>
#include <cstdlib>
#include <readline/readline.h>
#include <readline/history.h>
#include <atomic>
#include <csignal>

using namespace std;
atomic<bool> stop_stream{false};

void show_help() {
    cout << "使用方法:" << endl;
    cout << "  ag <消息>    # 单次查询" << endl;
    cout << "  ag           # 交互模式" << endl;
    cout << endl;
    cout << "配置 API 密钥:" << endl;
    cout << "  临时设置: export DEEPSEEK_API_KEY=\"密钥\"" << endl;
    cout << "  永久设置: echo 'export DEEPSEEK_API_KEY=\"密钥\"' >> ~/.bashrc 或 ~/.zshrc" << endl;
}

void handle_sigint(int) {
    stop_stream = true;
}

int main(int argc, char* argv[]) {
    std::signal(SIGINT, handle_sigint);

    LoggerInitializer::create("logs/agent.log")
        .set_console(false)
        .set_level(Logger::INFO)
        .set_max_size(10 * 1024);

    if (argc == 2 && (string(argv[1]) == "--help" || string(argv[1]) == "-h")) {
        show_help();
        return 0;
    }

    if (argc > 1) {
        string query;
        for (int i = 1; i < argc; i++) {
            if (i > 1) query += " ";
            query += argv[i];
        }
        log_info("单次查询模式, 问题: %s", query.c_str());
        Agent agent(5);
        string response = agent.send_msg(query);
        cout << response << endl;
        log_info("单次查询完成, 响应长度: %zu", response.length());
        return 0;
    }

    Agent agent(10);
    const char* api_key = getenv("DEEPSEEK_API_KEY");
    if (!api_key) {
        log_warn("未设置 DEEPSEEK_API_KEY, 使用模拟模式");
        cout << "⚠️ 未设置 DEEPSEEK_API_KEY，使用模拟模式" << endl;
        cout << "💡 运行 ag --help 查看设置方法" << endl;
    } else {
        log_info("API 密钥已设置, 长度: %zu", strlen(api_key));
    }

    log_info("进入交互模式");
    cout << "🤖 DeepSeek Chat, 有什么可以帮你?💬" << endl;

    while (true) {
        char* input_cstr = readline("👤 User: ");
        if (!input_cstr) {
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

    log_info("正常退出");
    return 0;
}