#pragma once
#include <string>
#include <functional>

class ApiClient {
public:
    using StreamCallback = std::function<void(const std::string&)>;

    ApiClient();
    ~ApiClient();

    void set_api_key(const std::string& api_key);
    void set_base_url(const std::string& base_url);
    
    std::string send_request(const std::string& context, StreamCallback callback = nullptr);  // 流式输出

    static std::string unescape_json_string(const std::string& str);

private:
    std::string m_api_key;
    std::string m_base_url;
};
