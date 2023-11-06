#include "define.h"

int create_client(const char* ip_address, int port)
{
    struct sockaddr_in multicast_addr;
    struct ip_mreq mreq;
    int fd;

    // 创建 UDP 套接字
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
        perror("socket");

    // 设置多播组地址
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_port = htons(port);
    multicast_addr.sin_addr.s_addr = inet_addr(ip_address);

    // 绑定套接字到接收多播数据的地址
    if (bind(fd, (struct sockaddr*)&multicast_addr, sizeof(multicast_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    // 加入多播组
    mreq.imr_multiaddr.s_addr = inet_addr(ip_address);
    mreq.imr_interface.s_addr = INADDR_ANY;
    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    return fd;
}

int main()
{
    int client = create_client(MULTICAST_GROUP, MULTICAST_PORT);

    while (1) {
        struct sockaddr_in sender_addr;
        socklen_t sender_addr_len = sizeof(sender_addr);
        char buffer[256] = { 0 };

        // 接收多播数据并获取源地址信息

        ssize_t bytes_received = recvfrom(client, buffer, sizeof(buffer), 0, (struct sockaddr*)&sender_addr, &sender_addr_len);
        if (bytes_received < 0) {
            perror("recvfrom");
            exit(1);
        }

        printf("Received from %s:%d: %s\n", inet_ntoa(sender_addr.sin_addr), ntohs(sender_addr.sin_port), buffer);
        sleep(SEND_INTERVAL);
    }

    return 0;
}