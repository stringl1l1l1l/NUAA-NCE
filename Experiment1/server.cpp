#include "define.h"
#include <stdio.h>

int create_server(const char* host, int port)
{
    struct sockaddr_in server;
    int fd;

    // 创建一个数据报套接字，返回其文件描述符（fd）
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
        perror("socket");

    // 允许套接字地址重用
    int option = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0)
        perror("setsockopt");

    // 设置服务器地址结构
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(host); // 绑定到所有网络接口

    // 绑定套接字到多播组地址
    if (bind(fd, (struct sockaddr*)&server, sizeof(server)) < 0)
        perror("bind");

    return fd;
}

void run_server(const char* ip_address, int port)
{
    int fd = create_server(ip_address, port);
    int counter = 0;

    while (1) {
        struct sockaddr_in multicast_addr;
        // 设置多播组地址
        memset(&multicast_addr, 0, sizeof(multicast_addr));
        multicast_addr.sin_family = AF_INET;
        multicast_addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
        multicast_addr.sin_port = htons(MULTICAST_PORT);

        // 构造要发送的数据
        char buffer[256] = { 0 };
        snprintf(buffer, sizeof(buffer), "Hello, Multicast!");

        // 发送数据到多播组
        if (sendto(fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&multicast_addr, sizeof(multicast_addr)) < 0) {
            perror("sendto");
            break;
        }
        
        printf("sendarrd:%s, port:%d\n", MULTICAST_GROUP, MULTICAST_PORT);
        printf("The message send is ***%s*** %d\n", buffer, counter++);
        
        sleep(SEND_INTERVAL);
    }

    close(fd);
}

int main(int argc, char* argv[])
{
    run_server(HOST, PORT);
    return 0;
}