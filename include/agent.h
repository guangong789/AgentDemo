#pragma once
#include "api_client.h"
#include <string>
#include <vector>
#include <functional>

class Agent {
public:
    using StreamCallback = std::function<void(const std::string&)>;

    Agent(size_t max_history = 10);
    std::string send_msg(const std::string& input, StreamCallback callback = nullptr);  // 流式输出

private:
    void check_limit();
    size_t m_history;
    std::vector<std::string> histories;
    ApiClient api_client;
};