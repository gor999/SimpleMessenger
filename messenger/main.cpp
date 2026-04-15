#include <QApplication>
#include "MessengerServer.hpp"
#include "MainWindow.hpp"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow w;
    MessengerServer server(8080);

    QObject::connect(&server, &MessengerServer::messageFromBrowser, &w, &MainWindow::displayBrowserMessage);
    QObject::connect(&server, &MessengerServer::systemLog, &w, &MainWindow::displayLog);
    QObject::connect(&server, &MessengerServer::heartbeat, &w, [&w](){ w.updateOnlineStatus(true); });
    QObject::connect(&w, &MainWindow::responseTyped, &server, &MessengerServer::sendResponseToBrowser);
    QObject::connect(&w, &MainWindow::clearHistoryRequested, &server, &MessengerServer::clearServerHistory);

    w.show();
    server.start();

    return a.exec();
}
