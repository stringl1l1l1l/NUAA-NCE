#include "define.h"
#include <stdio.h>

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

    // 加入多播组
    mreq.imr_multiaddr.s_addr = inet_addr(ip_address);
    mreq.imr_interface.s_addr = INADDR_ANY;
    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    // 绑定socket到接收多播数据的地址
    if (bind(fd, (struct sockaddr*)&multicast_addr, sizeof(multicast_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    return fd;
}

int main()
{
    int client = create_client(MULTICAST_GROUP, MULTICAST_PORT);
    
    int recv_counter = 0;
    int send_counter = 0;
    for (int i = 0; i < TEST_CNT; i++) {
        struct sockaddr_in sender_addr;
        char recv_buff[256] = {0};
        socklen_t sender_addr_len = sizeof(sender_addr);
        
        // 接收多播数据并获取源地址信息
        ssize_t bytes_received = recvfrom(client, recv_buff, sizeof(recv_buff), 0, (struct sockaddr*)&sender_addr, &sender_addr_len);
        if (bytes_received < 0) {
            perror("recvfrom");
            exit(1);
        }
        
        printf("Received from addr: %s, port: %d\n", inet_ntoa(sender_addr.sin_addr), ntohs(sender_addr.sin_port));
        printf("The received message is ***%s*** %d\n\n", recv_buff, recv_counter++);
        
        // 向服务器发送数据
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(SERVER_HOST);
        server_addr.sin_port = htons(SERVER_PORT);
        
        char send_buff[] = "Hello, server!";

        if (sendto(client, send_buff, strlen(send_buff), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("sendto");
            break;
        }
          
        printf("sendarrd:%s, port:%d\n", SERVER_HOST, SERVER_PORT);
        printf("The message send is ***%s*** %d\n\n", send_buff, send_counter++);
    }
    
    close(client);
    
    return 0;
}