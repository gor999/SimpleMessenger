#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <cstring>




int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Error in Bind! Port 8080 is anavailable." << std::endl;
        return 1;
    }
    listen(server_fd, 10);
    std::cout << "Server Starts. Waiting for messages at http://localhost:8080..." << std::endl;
    while (true) {
        int addrlen = sizeof(address);
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) continue;
        std::string buffer(30000,0);
        int bytes_read = recv(new_socket, buffer.data(), buffer.size(), 0);
        if (bytes_read > 0) {
            std::cout << "\n--- New message from browser ---" << std::endl;

            size_t pos = buffer.find("GET /?msg=");
            if (pos != std::string::npos) {
                std::string raw_msg = buffer.substr(pos + 10); 
                size_t space_pos = raw_msg.find(" ");
                std::string final_msg = raw_msg.substr(0, space_pos);

                std::cout << "Browser sent: " << final_msg << std::endl;
            } else {
                std::cout << "Direct page access (no message)" << std::endl;
            }
            std::cout << "=====" << std::endl;
        }
        std::string user_text;
        std::cout << "Write text for send to browser ))";
        std::getline(std::cin, user_text);         
        std::string html_body = 
            "<html><head><meta charset='UTF-8'></head><body>"
            "<h2>Write somethink to chat with browser.</h2>"
            "<form action='/' method='GET'>"
            "<input type='text' name='msg' autofocus>"
            "<input type='submit' value='Send to Terminal'>"
            "</form>"
            "<p>Last message: " + user_text + "</p>"
            "</body></html>";
        std::string response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=UTF-8\r\n"
            "Content-Length: " + std::to_string(html_body.length()) + "\r\n"
            "Connection: close\r\n"
            "\r\n" +
            html_body;
        send(new_socket, response.c_str(), response.length(), 0);
        close(new_socket);
    }
    return 0;
}
