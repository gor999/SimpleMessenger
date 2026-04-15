#include "../include/MessengerServer.hpp"
#include "../include/HttpUtils.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <QDateTime>

MessengerServer::MessengerServer(int port) : _port(port) {
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd < 0) throw std::runtime_error("Failed to create socket");

    int opt = 1;
    setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    _address.sin_family = AF_INET;
    _address.sin_addr.s_addr = INADDR_ANY;
    _address.sin_port = htons(_port);

    if (bind(_server_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
        throw std::runtime_error("Error in Bind! Port is unavailable.");
    }

    listen(_server_fd, 10);
}

MessengerServer::~MessengerServer() {
    if (_server_fd >= 0) close(_server_fd);
}

void MessengerServer::run() {
    emit systemLog("Server Started.", "#00ff00");
    emit systemLog("Local: http://localhost:" + QString::number(_port), "#00aaff");

    struct ifaddrs *ifap, *ifa;
    if (getifaddrs(&ifap) == 0) {
        for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
                struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
                char *addr = inet_ntoa(sa->sin_addr);
                if (std::string(addr) != "127.0.0.1") {
                    emit systemLog("Network: http://" + QString(addr) + ":" + QString::number(_port), "#ffa500");
                }
            }
        }
        freeifaddrs(ifap);
    }

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);
        int new_socket = accept(_server_fd, (struct sockaddr *)&client_addr, &addrlen);
        
        if (new_socket >= 0) {
            _handleConnection(new_socket);
        }
    }
}

void MessengerServer::sendResponseToBrowser(QString text) {
    _mutex.lock();
    ChatEntry entry = {text.toStdString(), false, QDateTime::currentDateTime().toString("HH:mm").toStdString()};
    _chatHistory.push_back(entry);
    _mutex.unlock();
}

void MessengerServer::clearServerHistory() {
    _mutex.lock();
    _chatHistory.clear();
    _mutex.unlock();
    emit systemLog("Chat history cleared by admin.", "#ff5555");
}

void MessengerServer::_handleConnection(int client_fd) {
    std::string buffer(30000, 0);
    int bytes_read = recv(client_fd, &buffer[0], buffer.size(), 0);

    if (bytes_read > 0) {
        emit heartbeat();

        size_t get_pos = buffer.find("GET ");
        size_t end_pos = buffer.find(" HTTP/1.1");
        std::string path = (get_pos != std::string::npos && end_pos != std::string::npos) 
                           ? buffer.substr(get_pos + 4, end_pos - (get_pos + 4)) : "/";

        if (path != "/" && path.find("/?msg=") == std::string::npos && path != "/favicon.ico") {
            std::string filename = path.substr(1);
            std::ifstream file(filename.c_str(), std::ios::binary | std::ios::ate);
            if (file.is_open()) {
                std::streamsize size = file.tellg();
                file.seekg(0, std::ios::beg);
                std::vector<char> file_buf(size);
                if (file.read(file_buf.data(), size)) {
                    std::string head = "HTTP/1.1 200 OK\r\nContent-Type: " + HttpUtils::getMimeType(path) + "\r\nContent-Length: " + std::to_string(size) + "\r\nConnection: close\r\n\r\n";
                    send(client_fd, head.c_str(), head.length(), 0);
                    send(client_fd, file_buf.data(), size, 0);
                }
            }
            close(client_fd);
            return;
        }

        size_t pos = buffer.find("GET /?msg=");
        if (pos != std::string::npos) {
            std::string raw_msg = buffer.substr(pos + 10);
            size_t space_pos = raw_msg.find(" ");
            std::string decoded = HttpUtils::urlDecode(raw_msg.substr(0, space_pos));
            
            _mutex.lock();
            ChatEntry entry = {decoded, true, QDateTime::currentDateTime().toString("HH:mm").toStdString()};
            _chatHistory.push_back(entry);
            _mutex.unlock();
            
            emit messageFromBrowser(QString::fromStdString(decoded));
        }
    }

    _mutex.lock();
    std::vector<ChatEntry> historyCopy = _chatHistory;
    _mutex.unlock();

    std::string html = HttpUtils::generateHtmlResponse(historyCopy);
    std::string full_response = HttpUtils::buildFullHttpResponse(html);

    send(client_fd, full_response.c_str(), full_response.length(), 0);
    close(client_fd);
}
