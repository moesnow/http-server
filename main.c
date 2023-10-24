#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

// 用于从客户端接收数据的线程函数
void *readClientData(void *arg) {
    long fd = (long)arg;  // 获取套接字文件描述符
    char buf[1024] = {};

    for (;;) {
        int ret = recv(fd, buf, 1024, 0);  // 从客户端接收数据
        if (ret <= 0) {
            break;
        }
        puts(buf);  // 在服务器端打印接收到的数据

        // 构建HTTP响应消息，回复一个简单的HTML页面
        char html[1024] = "<html>\n<head>\n<title>title</title>\n</head>\n<body>Hello,World!</body>\n</html>";
        // 构建HTTP响应消息的头
        int len = strlen(html);
        char head[1024] = {};
        sprintf(head, "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nContent-Length:%d\r\n\r\n", len);

        // 发送HTTP响应消息给客户端
        send(fd, head, strlen(head), 0);
        send(fd, html, strlen(html), 0);
    }

    return NULL;
}

// 错误处理函数，打印错误信息并退出
void errExit(const char *desc) {
    perror(desc);
    exit(-1);
}

// 处理客户端连接的函数
void acceptClient(int lifd) {
    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);

    for (;;) {
        int cfd = accept(lifd, (struct sockaddr *)&addr, &addrLen);  // 接受客户端连接
        pthread_t id;
        pthread_create(&id, NULL, readClientData, (void *)(long)cfd);  // 创建一个新线程处理客户端数据
    }
}

// HTTP服务器的主函数
void httpServer(const char *ip, unsigned short port) {
    int lifd = socket(AF_INET, SOCK_STREAM, 0);  // 创建套接字
    if (lifd == -1) {
        errExit("socket");  // 错误处理
    }

    // 配置 IP 地址和端口
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    int ret = 0;
    ret = bind(lifd, (const struct sockaddr *)&addr, sizeof(addr));  // 绑定套接字到指定IP地址和端口
    if (ret != 0) {
        errExit("bind");  // 错误处理
    }

    ret = listen(lifd, 5);  // 监听客户端连接请求
    if (ret != 0) {
        errExit("listen");  // 错误处理
    }

    acceptClient(lifd);  // 处理客户端连接
}

// 主函数，从命令行参数获取IP地址和端口号，启动HTTP服务器
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("usage: %s <ip> <port>\n", argv[0]);
        return -1;
    }
    httpServer(argv[1], atoi(argv[2]));
    return 0;
}
