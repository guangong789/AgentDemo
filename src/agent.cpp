#include "agent.h"
#include <sstream>
#include <algorithm>

std::string Agent::send_msg(const std::string& input, StreamCallback callback) {
    std::string response = api_client.send_request(input, callback);

    return response;
}