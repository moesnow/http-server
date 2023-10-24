#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <thread>

class HttpServer {
   public:
    HttpServer(char* ip, int port) {
        this->ip = ip;
        this->port = port;
    }

    void start() {
        // 创建套接字
        int lifd = socket(AF_INET, SOCK_STREAM, 0);
        if (lifd < 0) {
            return;
        }

        // 绑定套接字
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip);
        if (bind(lifd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            return;
        }

        // 监听套接字
        if (listen(lifd, 10) < 0) {
            return;
        }

        // 处理客户端连接
        AcceptClients(lifd);
    }

   private:
    char* ip;
    int port;

    void ReadClientData(int clientfd) {
        // 读取客户端数据
        char buf[1024];
        while (true) {
            int ret = read(clientfd, buf, sizeof(buf));
            if (ret <= 0) {
                return;
            }
            // 输出客户端数据
            std::cout << buf << std::endl;

            // 发送响应
            std::string html = "<html>\n<head>\n<title>title</title>\n</head>\n<body>Hello, World!</body>\n</html>";
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(html.size()) + "\r\n\r\n" + html;

            write(clientfd, response.c_str(), response.size());
        }
    }

    void AcceptClients(int lifd) {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        while (true) {
            // 接受客户端连接
            int clientfd = accept(lifd, (struct sockaddr*)&addr, &len);
            if (clientfd < 0) {
                return;
            }
            // 创建线程
            std::thread clientThread(&HttpServer::ReadClientData, this, clientfd);
            // 设置线程为守护线程
            clientThread.detach();
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "usage: " << argv[0] << " <ip> <port>" << std::endl;
        return -1;
    }
    HttpServer server(argv[1], atoi(argv[2]));
    server.start();
}
