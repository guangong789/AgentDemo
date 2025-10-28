#include "api_client.h"
#include <iostream>
#include <sstream>
#include <curl/curl.h>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <atomic>
extern std::atomic<bool> stop_stream;

using json = nlohmann::json;
using namespace std;

static bool extract_and_print_delta(string& buf, ApiClient::StreamCallback callback) {
    const string key = "\"delta\":{\"content\":\"";
    size_t pos = buf.find(key);
    if (pos == string::npos) return false;
    pos += key.size();

    string raw;
    bool escape = false;
    for (size_t i = pos; i < buf.size(); ++i) {
        char c = buf[i];
        if (escape) {
            raw.push_back(c);
            escape = false;
        } else if (c == '\\') {
            escape = true;
        } else if (c == '"') {
            string sub = buf.substr(pos, i - pos);
            string unescaped = ApiClient::unescape_json_string(sub);
            if (callback)
                callback(unescaped);
            else
                cout << unescaped << flush;

            buf.erase(0, i + 1);
            return true;
        } else {
            raw.push_back(c);
        }
    }
    return false;
}

static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    if (stop_stream.load()) {
        return 0;
    }

    size_t total_size = size * nmemb;
    string chunk((char*)contents, total_size);

    struct CallbackData {
        string* response;
        ApiClient::StreamCallback callback;
        string buffer;
    };

    CallbackData* data = static_cast<CallbackData*>(userp);
    data->response->append(chunk);
    data->buffer += chunk;

    while (!stop_stream.load() && extract_and_print_delta(data->buffer, data->callback)) {}

    return total_size;
}

ApiClient::ApiClient() {
    m_base_url = "https://api.deepseek.com/v1";
    m_api_key = load_api_key();
}

void ApiClient::set_api_key(const string& api_key) {
    m_api_key = api_key;
}
void ApiClient::set_base_url(const string& base_url) {
    m_base_url = base_url;
}

string ApiClient::send_request(const string& context, StreamCallback callback) {
    if (m_api_key.empty()) return "模拟响应: 请设置 API KEY";

    json jreq = {
        {"model", "deepseek-chat"},
        {"messages", {{{"role", "user"}, {"content", context}}}},
        {"max_tokens", 800},
        {"temperature", 0.7},
        {"stream", true}};
    string request_body = jreq.dump();

    CURL* curl = curl_easy_init();
    if (!curl) return "无法初始化 CURL";

    string response_string;
    struct {
        string* response;
        ApiClient::StreamCallback callback;
        string buffer;
    } data = {&response_string, callback, ""};

    curl_easy_setopt(curl, CURLOPT_URL, (m_base_url + "/chat/completions").c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120L);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("Authorization: Bearer " + m_api_key).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) return "网络错误: " + string(curl_easy_strerror(res));
    return response_string;
}

string ApiClient::unescape_json_string(const string& str) {
    string result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            char next = str[i + 1];
            switch (next) {
            case 'n': result += '\n'; break;
            case 'r': result += '\r'; break;
            case 't': result += '\t'; break;
            case '"': result += '"'; break;
            case '\\': result += '\\'; break;
            default: result += next; break;
            }
            i++;
        } else
            result += str[i];
    }
    return result;
}

std::string ApiClient::get_home_dir() {
    const char* home = getenv("HOME");
    if (home) return std::string(home);
    struct passwd* pw = getpwuid(getuid());
    return pw ? std::string(pw->pw_dir) : ".";
}

std::string ApiClient::load_api_key() {
    // 环境变量
    if (const char* env_key = getenv("DEEPSEEK_API_KEY")) {
        return std::string(env_key);
    }
    // 配置文件
    std::string config_path = get_home_dir() + "/.deepseek_config";
    std::ifstream fin(config_path);
    if (fin) {
        std::string key;
        std::getline(fin, key);
        if (!key.empty()) return key;
    }
    // 用户输入
    std::string key;
    std::cout << "🔑 请输入 DEEPSEEK_API_KEY(留空模拟模式): ";
    std::getline(std::cin, key);
    if (!key.empty()) {
        std::cout << "是否保存到本地配置文件 (" << config_path << ")? [y/N]: ";
        std::string ans;
        std::getline(std::cin, ans);
        if (ans == "y" || ans == "Y") {
            std::ofstream fout(config_path);
            fout << key;
            std::cout << "✅ 已保存 API Key.\n";
        }
    }
    return key;
}