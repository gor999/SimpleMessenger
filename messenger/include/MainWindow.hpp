#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

public slots:
    void displayBrowserMessage(QString msg);
    void displayLog(QString msg, QString color);
    void handleSend();
    void handlePickImage();
    void clearHistory();
    void updateOnlineStatus(bool online);

signals:
    void responseTyped(QString text);
    void clearHistoryRequested();

private:
    QTextEdit *_history;
    QLineEdit *_input;
    QPushButton *_sendBtn;
    QPushButton *_imageBtn;
    QPushButton *_clearBtn;
    QLabel *_headerLabel;
    QWidget *_headerWidget;
    QLabel *_statusLabel;
    QTimer *_offlineTimer;
};

#endif