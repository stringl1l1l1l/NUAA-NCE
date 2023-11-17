#include "define.h"
#include <stdio.h>
#include <string.h>

struct Address {
    char hostname[20];
    char address[20];
    int port;
    int open;
} addr[NUMBER_OF_LAN + 1][NUMBER_OF_PORTS + 1];

void TranAddr(char* ad)
{
    struct sockaddr_in saddr;
    // 调用inet_aton()，将ptr点分十进制转in_addr
    if (!inet_aton(ad, &saddr.sin_addr)) {
        printf("Inet_aton error\n");
    }

    // 获取前缀
    char* pos = NULL;
    for (int i = 0; ad[i]; i++) {
        if (ad[i] == '.')
            pos = ad + i;
    }
    if (pos)
        *pos = 0;
    
    // 遍历子网
    for (int i = 1; i <= NUMBER_OF_LAN; i++) {
        char scan_addr[50] = { 0 };
        struct in_addr saddr;

        sprintf_s(scan_addr, "%s.%d", ad, i);
    
        inet_aton(scan_addr, &saddr);

        struct hostent* host = gethostbyaddr((void*)&saddr, sizeof(saddr), AF_INET);
        if (host == NULL) {
            // strcpy(addr[i][0].address, scan_addr);
            // addr[i][0].port = -1;
            // addr[i][0].open = PORT_CLOSE;
            printf("invalid ip: %s\n", addr[i][0].address);
            continue;
        }

        for (int j = 0; j <= NUMBER_OF_PORTS; j++) {
            strcpy(addr[i][j].hostname, host->h_name);
            strcpy(addr[i][j].address, scan_addr);
            addr[i][j].port = j;
            addr[i][j].open = PORT_CLOSE;
            printf("name: %s, ip: %s, port: %d\n", addr[i][j].hostname, addr[i][j].address, addr[i][j].port);
        }
    }
}

void sockconnect(struct Address* b)
{
}

void* pth_main(void* addr)
{
    return NULL;
}

int main(int argc, char** argv)
{
    char addr_str[] = "127.0.0.1";
    if (argv && argv[0]) {
        strcpy(addr_str, argv[1]);
    }
    TranAddr(addr_str);

    // pthread_t pth_main_tid;
    // pthread_create(&pth_main_tid, NULL, pth_main, &addr);
    return 0;
}