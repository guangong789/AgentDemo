#pragma once
#include "api_client.h"
#include <string>
#include <vector>
#include <functional>
#include <regex>

class Agent {
public:
    using StreamCallback = std::function<void(const std::string&)>;

    Agent() = default;
    std::string send_msg(const std::string& input, StreamCallback callback = nullptr);

private:
    ApiClient api_client;
};