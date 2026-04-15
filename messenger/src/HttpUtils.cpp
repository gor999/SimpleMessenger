#include "../include/HttpUtils.hpp"
#include <sstream>
#include <iomanip>

std::string HttpUtils::urlDecode(std::string str) {
    std::string res;
    char c;
    int j;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '+') {
            res += ' ';
        } else if (str[i] == '%' && i + 2 < str.length() && isxdigit(str[i + 1]) && isxdigit(str[i + 2])) {
            std::stringstream ss;
            ss << std::hex << str.substr(i + 1, 2);
            ss >> j;
            c = static_cast<char>(j);
            res += c;
            i += 2;
        } else {
            res += str[i];
        }
    }
    return res;
}

std::string HttpUtils::generateHtmlResponse(const std::vector<ChatEntry>& history) {
    std::string historyHtml = "";
    for (size_t i = 0; i < history.size(); ++i) {
        std::string content = history[i].text;
        if (!history[i].isFromBrowser) {
            size_t dot = content.find_last_of(".");
            if (dot != std::string::npos) {
                std::string ext = content.substr(dot);
                if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".gif") {
                    content = "<img src='/" + content + "' style='max-width:100%; border-radius:8px; margin-top:5px;'><br><b>File:</b> " + content;
                }
            }
        }
        std::string bubbleClass = history[i].isFromBrowser ? "browser-bubble" : "terminal-bubble";
        std::string label = history[i].isFromBrowser ? "You" : "Server";
        std::string time = history[i].timestamp;
        historyHtml += "<div class='bubble " + bubbleClass + "'><div class='label'>" + label + "</div>" + content + "<div style='font-size:0.7rem; color:rgba(255,255,255,0.5); text-align:right; margin-top:4px;'>" + time + "</div></div>";
    }

    return "<!DOCTYPE html><html><head>"
           "<meta charset='UTF-8'>"
           "<meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no'>"
           "<style>"
           "* { box-sizing: border-box; }"
           "body { font-family: 'Segoe UI', Roboto, sans-serif; background-color: #0e1621; color: #f5f5f5; margin: 0; display: flex; flex-direction: column; height: 100vh; overflow: hidden; }"
           ".app-header { background-color: #17212b; padding: 15px; text-align: center; box-shadow: 0 2px 5px rgba(0,0,0,0.3); z-index: 10; }"
           ".app-header h2 { margin: 0; font-size: 1.1rem; font-weight: 500; color: #fff; }"
           ".history { flex: 1; overflow-y: auto; padding: 20px; display: flex; flex-direction: column; gap: 12px; scroll-behavior: smooth; padding-bottom: 90px; }"
           ".bubble { max-width: 85%; padding: 10px 14px; border-radius: 16px; font-size: 0.95rem; line-height: 1.4; position: relative; word-wrap: break-word; box-shadow: 0 1px 2px rgba(0,0,0,0.3); animation: fadeIn 0.2s ease-in; }"
           ".browser-bubble { align-self: flex-end; background-color: #2b5278; border-bottom-right-radius: 4px; }"
           ".terminal-bubble { align-self: flex-start; background-color: #182533; border-bottom-left-radius: 4px; }"
           ".label { font-size: 0.75rem; color: #3390ec; font-weight: bold; margin-bottom: 4px; display: block; }"
           ".input-container { position: fixed; bottom: 0; left: 0; right: 0; background-color: #17212b; padding: 12px; display: flex; gap: 10px; align-items: center; }"
           "input[type='text'] { flex: 1; background-color: #242f3d; border: 1px solid #242f3d; color: white; padding: 12px 18px; border-radius: 25px; outline: none; font-size: 1rem; transition: border 0.3s; }"
           "input[type='text']:focus { border: 1px solid #3390ec; }"
           "button { background-color: #3390ec; border: none; color: white; width: 45px; height: 45px; border-radius: 50%; cursor: pointer; display: flex; align-items: center; justify-content: center; transition: background 0.2s; }"
           "button:active { background-color: #2b84d6; transform: scale(0.95); }"
           "img { max-width: 100%; border-radius: 10px; display: block; margin-top: 5px; }"
           "@keyframes fadeIn { from { opacity: 0; transform: translateY(5px); } to { opacity: 1; transform: translateY(0); } }"
           "</style></head>"
           "<body>"
           "<div class='app-header'><h2>Fake Telegram</h2></div>"
           "<div class='history'>"
           + (historyHtml.empty() ? "<div class='bubble terminal-bubble'>No messages yet.</div>" : historyHtml) +
           "</div>"
           "<form id='chatForm' class='input-container'>"
           "<input type='text' name='msg' placeholder='Message' autofocus required autocomplete='off'>"
           "<button type='submit'>"
           "<svg width='24' height='24' viewBox='0 0 24 24' fill='none' stroke='currentColor' stroke-width='2' stroke-linecap='round' stroke-linejoin='round'><line x1='22' y1='2' x2='11' y2='13'></line><polygon points='22 2 15 22 11 13 2 9 22 2'></polygon></svg>"
           "</button>"
           "</form>"
           "<script>"
           "const history = document.querySelector('.history');"
           "const form = document.getElementById('chatForm');"
           "const input = form.querySelector('input');"
           "history.scrollTop = history.scrollHeight;"
           "form.onsubmit = async (e) => {"
           "    e.preventDefault();"
           "    const msg = input.value;"
           "    if(!msg) return;"
           "    input.value = '';"
           "    await fetch('/?msg=' + encodeURIComponent(msg));"
           "    updateChat();"
           "};"
           "async function updateChat() {"
           "    try {"
           "        const res = await fetch('/');"
           "        const text = await res.text();"
           "        const parser = new DOMParser();"
           "        const doc = parser.parseFromString(text, 'text/html');"
           "        const newHistory = doc.querySelector('.history').innerHTML;"
           "        if (history.innerHTML !== newHistory) {"
           "            history.innerHTML = newHistory;"
           "            history.scrollTo({ top: history.scrollHeight, behavior: 'smooth' });"
           "        }"
           "    } catch(e) {}"
           "}"
           "setInterval(updateChat, 2000);"
           "</script>"
           "</body></html>";
}

std::string HttpUtils::buildFullHttpResponse(const std::string& htmlContent) {
    std::string response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Content-Length: " + std::to_string(htmlContent.length()) + "\r\n"
        "Connection: close\r\n"
        "\r\n" +
        htmlContent;
    return response;
}

std::string HttpUtils::getMimeType(const std::string& path) {
    if (path.find(".png") != std::string::npos) return "image/png";
    if (path.find(".jpg") != std::string::npos || path.find(".jpeg") != std::string::npos) return "image/jpeg";
    if (path.find(".gif") != std::string::npos) return "image/gif";
    if (path.find(".html") != std::string::npos) return "text/html";
    if (path.find(".css") != std::string::npos) return "text/css";
    return "application/octet-stream";
}