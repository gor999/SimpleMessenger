Fake Telegram
A cross-platform messaging application featuring a Telegram-style desktop client and a responsive web interface. This project demonstrates a hybrid architecture where a C++ backend handles concurrent connections, manages chat history, and serves web content.

## Features

- **Telegram-Inspired UI**: A polished desktop interface built with Qt5, featuring dark mode, message bubbles, and avatars.
- **Responsive Web Client**: A mobile-friendly web interface that allows users to chat from their phones via a local network.
- **Real-time Updates**: Implements an asynchronous heartbeat and polling mechanism to keep all clients in sync.
- **Status Tracking**: Visual online/offline indicators for connected browser users.
- **Persistent Session History**: Shared in-memory chat history available to both desktop and web participants.
- **Network Discovery**: Automatically displays the local IP address for easy mobile connections.

## Tech Stack

- **Desktop**: C++11, Qt5 (Widgets, Gui, Core)
- **Backend**: Socket programming (C/C++), Multi-threading (QThread)
- **Web**: HTML5, CSS3, JavaScript (Fetch API)

## Getting Started

1. **Compile**: Run `make` in the root directory.
2. **Run**: Execute `./messenger`.
3. **Connect**: Open your browser to `http://localhost:8080`.
4. **Mobile**: Use the IP address displayed in the Qt console to connect from your phone.
