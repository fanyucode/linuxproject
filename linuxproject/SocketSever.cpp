#include "SocketServer.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <sys/epoll.h>

int server_fd, new_socket;
struct sockaddr_in address;
int epoll_fd;
struct epoll_event ev, events[10];  

void initializeSocketServer() {
    // 创建 socket 文件描述符
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // 绑定并监听
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // 设置 server socket 为非阻塞模式
    int flags = fcntl(server_fd, F_GETFL, 0);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);

    // 创建 epoll 实例
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        perror("epoll_ctl: server_fd");
        exit(EXIT_FAILURE);
    }
}

void sendTemperatureAlert() {
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        return;
    }
    const char *message = "温度警告";
    send(new_socket, message, strlen(message), 0);
    close(new_socket);
}

void sendSoundAlert() {
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        return;
    }
    const char *message = "声音警告";
    send(new_socket, message, strlen(message), 0);
    close(new_socket);
}

void closeSocketServer() {
    close(server_fd);
    close(epoll_fd);
}

void handleClientCommand(const std::string& command) {
    if (command.find("SET_ANGLE:") != std::string::npos) {
        int angle = std::stoi(command.substr(10));
        setServoAngle(angle);  // 调用舵机控制函数
    }
}

void handleConnections() {
    int nfds = epoll_wait(epoll_fd, events, 10, -1);
    if (nfds == -1) {
        perror("epoll_wait");
        exit(EXIT_FAILURE);
    }

    for (int n = 0; n < nfds; ++n) {
        if (events[n].data.fd == server_fd) {
            new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&address);
            if (new_socket == -1) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            int flags = fcntl(new_socket, F_GETFL, 0);
            fcntl(new_socket, F_SETFL, flags | O_NONBLOCK);

            ev.events = EPOLLIN | EPOLLET;
            ev.data.fd = new_socket;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &ev) == -1) {
                perror("epoll_ctl: new_socket");
                exit(EXIT_FAILURE);
            }
        } else {
            // 处理客户端数据
            char buffer[1024] = {0};
            int valread = read(events[n].data.fd, buffer, 1024);
            if (valread == 0) {
                // 客户端连接关闭
                close(events[n].data.fd);
            } else {
                
                handleClientCommand(buffer);
            }
        }
    }
}
