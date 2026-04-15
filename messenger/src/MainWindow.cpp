#include "MainWindow.hpp"
#include <QFileDialog>
#include <QFileInfo>
#include <QScreen>
#include <QGuiApplication>
#include <QScrollBar>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    this->setStyleSheet(
        "QMainWindow { background-color: #0e1621; }"
        "QTextEdit { background-color: #0e1621; color: #f5f5f5; border: none; padding: 10px; font-family: 'Segoe UI', sans-serif; font-size: 14px; }"
        "QLineEdit { background-color: #17212b; color: white; border-radius: 20px; padding: 8px 15px; font-size: 14px; selection-background-color: #2b5278; border: none; }"
        "QLineEdit:focus { border: 1px solid #3390ec; }"
        "QPushButton { background-color: #3390ec; color: #ffffff; border: none; border-radius: 18px; padding: 8px 15px; font-weight: bold; min-width: 70px; }"
        "QPushButton:hover { background-color: #2b84d6; }"
        "QPushButton#clearBtn { background-color: transparent; color: #e53935; border: none; min-width: 60px; }"
        "QPushButton#clearBtn:hover { color: #cf6679; }"
        "QScrollBar:vertical { border: none; background: #0e1621; width: 10px; margin: 0px; }"
        "QScrollBar::handle:vertical { background: #242f3d; min-height: 20px; border-radius: 5px; }"
        "QScrollBar::handle:vertical:hover { background: #3390ec; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
    );

    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    _headerWidget = new QWidget();
    _headerWidget->setFixedHeight(60);
    _headerWidget->setStyleSheet("background-color: #17212b; border-bottom: 1px solid #0e1621;");
    
    QHBoxLayout *headerMainLayout = new QHBoxLayout(_headerWidget);
    headerMainLayout->setContentsMargins(15, 0, 15, 0);
    headerMainLayout->setSpacing(15);

    QLabel *avatarLabel = new QLabel("B");
    avatarLabel->setFixedSize(40, 40);
    avatarLabel->setAlignment(Qt::AlignCenter);
    avatarLabel->setStyleSheet("background-color: #3390ec; color: white; border-radius: 20px; font-weight: bold; font-size: 18px; border: none;");

    QVBoxLayout *headerTextLayout = new QVBoxLayout();
    _headerLabel = new QLabel("Browser User");
    _headerLabel->setStyleSheet("color: white; font-weight: bold; font-size: 15px; border: none; padding: 0; margin: 0;");
    _statusLabel = new QLabel("offline");
    _statusLabel->setStyleSheet("color: #888888; font-size: 12px; border: none; padding: 0; margin: 0;");
    headerTextLayout->addWidget(_headerLabel, 0, Qt::AlignBottom);
    headerTextLayout->addWidget(_statusLabel, 0, Qt::AlignTop);

    headerMainLayout->addWidget(avatarLabel);
    headerMainLayout->addLayout(headerTextLayout);
    headerMainLayout->addStretch();

    _offlineTimer = new QTimer(this);
    _offlineTimer->setSingleShot(true);
    connect(_offlineTimer, &QTimer::timeout, [this]() { updateOnlineStatus(false); });

    _history = new QTextEdit();
    _history->setReadOnly(true);
    _history->setPlaceholderText("Chat history will appear here...");

    QWidget *bottomArea = new QWidget();
    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomArea);
    bottomLayout->setContentsMargins(10, 10, 10, 10);
    bottomLayout->setSpacing(10);

    _input = new QLineEdit();
    _input->setPlaceholderText("Write a message...");
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    _sendBtn = new QPushButton("➤");
    _imageBtn = new QPushButton("📎");
    _clearBtn = new QPushButton("Clear");
    _clearBtn->setObjectName("clearBtn");

    buttonLayout->addWidget(_imageBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(_clearBtn);
    buttonLayout->addWidget(_sendBtn);

    bottomLayout->addWidget(_input);
    bottomLayout->addLayout(buttonLayout);

    layout->addWidget(_headerWidget);
    layout->addWidget(_history);
    layout->addWidget(bottomArea);

    setCentralWidget(central);
    // Mobile apps should always be full screen
    this->showMaximized();
    setWindowTitle("Fake Telegram");

    connect(_sendBtn, &QPushButton::clicked, this, &MainWindow::handleSend);
    connect(_input, &QLineEdit::returnPressed, this, &MainWindow::handleSend);
    connect(_imageBtn, &QPushButton::clicked, this, &MainWindow::handlePickImage);
    connect(_clearBtn, &QPushButton::clicked, this, &MainWindow::clearHistory);
}

void MainWindow::displayBrowserMessage(QString msg) {
    _history->append("<div align='left'><div style='background-color: #182533; color: white; border-radius: 12px; border-bottom-left-radius: 2px; padding: 12px; margin-bottom: 5px;'> " + msg + " </div></div>");
}

void MainWindow::displayLog(QString msg, QString color) {
    _history->append("<div align='center'><span style='color: " + color + "; font-size: 11px; font-weight: bold; background-color: rgba(0,0,0,0.3); padding: 4px 10px; border-radius: 10px;'> " + msg.toUpper() + " </span></div>");
}

void MainWindow::updateOnlineStatus(bool online) {
    if (online) {
        _statusLabel->setText("online");
        _statusLabel->setStyleSheet("color: #3390ec; font-size: 13px; border: none;");
        _offlineTimer->start(5000);
    } else {
        _statusLabel->setText("offline");
        _statusLabel->setStyleSheet("color: #888888; font-size: 13px; border: none;");
    }
}

void MainWindow::handleSend() {
    QString time = QDateTime::currentDateTime().toString("HH:mm");
    QString text = _input->text();
    if (text.isEmpty()) text = " ";
    _history->append("<table width='100%'><tr><td width='60'></td><td align='right'><div style='background-color: #2b5278; color: white; border-radius: 12px; border-bottom-right-radius: 2px; padding: 10px;'>" 
                     + text + "<br><small style='color: #aab; font-size: 10px;'>" + time + "</small></div></td></tr></table>");
    emit responseTyped(text);
    _input->clear();
    _history->verticalScrollBar()->setValue(_history->verticalScrollBar()->maximum());
}

void MainWindow::handlePickImage() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Image"), "", tr("Images (*.png *.jpg *.jpeg *.gif)"));
    if (!fileName.isEmpty()) {
        QFileInfo info(fileName);
        // We send just the filename as the server expects files to be in the working directory
        QString shortName = info.fileName();
        _input->setText(shortName);
        handleSend();
    }
}

void MainWindow::clearHistory() {
    _history->clear();
    emit clearHistoryRequested();
}
