#include "agent.h"
#include <algorithm>
using namespace std;

Agent::Agent(size_t max_history) : m_history(max_history) {}

void Agent::check_limit() {
    if (histories.size() > m_history) {
        histories.erase(histories.begin());
    }
}

string Agent::send_msg(const string& input, StreamCallback callback) {
    histories.push_back("User: " + input);
    check_limit();

    string context;
    for (const auto& msg : histories) context += msg + "\n";

    string response = api_client.send_request(context, callback);

    histories.push_back("Assistant: " + response);
    check_limit();
    return response;
}