#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

void *readClientData(void *arg) {
    long fd = (long)arg;
    char buf[1024] = {};
    for (;;) {
        int ret = recv(fd, buf, 1024, 0);
        if (ret <= 0) {
            break;
        }
        puts(buf);
        char html[1024] = "<html>\n<head>\n<title>title</title>\n</head>\n<body>Hello,World!</body>\n</html>";
        int len = strlen(html);
        char head[1024] = {};
        sprintf(head, "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nContent-Length:%d\r\n\r\n", len);
        send(fd, head, strlen(head), 0);
        send(fd, html, len, 0);
    }
    return NULL;
}

void errExit(const char *desc) {
    perror(desc);
    exit(-1);
}

void acceptClient(int lifd) {
    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);
    for (;;) {
        int cfd = accept(lifd, (struct sockaddr *)&addr, &addrLen);
        pthread_t id;
        pthread_create(&id, NULL, readClientData, (void *)(long)cfd);
    }
}

void httpServer(const char *ip, unsigned short port) {
    int lifd = socket(AF_INET, SOCK_STREAM, 0);
    if (lifd == -1) {
        errExit("socket");
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    int ret = 0;
    ret = bind(lifd, (const struct sockaddr *)&addr, sizeof(addr));
    if (ret != 0) {
        errExit("bind");
    }

    ret = listen(lifd, 5);
    if (ret != 0) {
        errExit("listen");
    }

    acceptClient(lifd);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("usage:%s <ip> <port>\n", argv[0]);
        return -1;
    }
    httpServer(argv[1], atoi(argv[2]));
    return 0;
}