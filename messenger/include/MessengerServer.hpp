#ifndef MESSENGER_SERVER_HPP
#define MESSENGER_SERVER_HPP

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <netinet/in.h>
#include <string>
#include <vector>
#include "HttpUtils.hpp"

class MessengerServer : public QThread {
    Q_OBJECT

public:
    MessengerServer(int port);
    ~MessengerServer();

    void run() override;

signals:
    void messageFromBrowser(QString msg);
    void systemLog(QString msg, QString color);
    void heartbeat();

public slots:
    void sendResponseToBrowser(QString text);
    void clearServerHistory();

private:
    int _server_fd;
    int _port;
    struct sockaddr_in _address;
    
    QMutex _mutex;
    std::vector<ChatEntry> _chatHistory;

    void _handleConnection(int client_fd);
};

#endif