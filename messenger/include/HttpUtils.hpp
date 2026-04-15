#ifndef HTTP_UTILS_HPP
#define HTTP_UTILS_HPP

#include <string>
#include <vector>

struct ChatEntry {
    std::string text;
    bool isFromBrowser;
    std::string timestamp;
};

class HttpUtils {
public:
    static std::string urlDecode(std::string str);
    static std::string generateHtmlResponse(const std::vector<ChatEntry>& history);
    static std::string buildFullHttpResponse(const std::string& htmlContent);
    static std::string getMimeType(const std::string& path);
};

#endif