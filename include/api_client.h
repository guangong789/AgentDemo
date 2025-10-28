#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <functional>

class ApiClient {
public:
    using StreamCallback = std::function<void(const std::string&)>;

    ApiClient();
    ~ApiClient() = default;

    void set_api_key(const std::string& api_key);
    void set_base_url(const std::string& base_url);
    
    std::string send_request(const std::string& context, StreamCallback callback = nullptr);
    static std::string unescape_json_string(const std::string& str);

private:
    std::string m_api_key;
    std::string m_base_url;

    std::string get_home_dir();
    std::string load_api_key();
};
