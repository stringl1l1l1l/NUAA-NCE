#include "define.h"
#include <cstring>
#include <stdio.h>
#include <string.h>
#include <vector>

struct Address {
    char hostname[20];
    char address[20];
    int port;
    int open;
};

Address** TranAddr(char* ad)
{
    Address** addr = new Address*[NUMBER_OF_LAN + 1];
    memset(addr, 0, sizeof(Address*) * (NUMBER_OF_LAN + 1));

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
        char scan_addr[100] = { 0 };
        snprintf(scan_addr, sizeof(scan_addr), "%s.%d", ad, i);

        struct in_addr saddr;
        inet_aton(scan_addr, &saddr);

        addr[i] = new Address[NUMBER_OF_PORTS + 1];
        addr[i][0].port = 0;

        struct hostent* host = gethostbyaddr((void*)&saddr, sizeof(saddr), AF_INET);
        if (host == NULL) {
            strcpy(addr[i][0].hostname, "nameless");
            strcpy(addr[i][0].address, scan_addr);
            addr[i][0].port = -1;
            addr[i][0].open = PORT_CLOSE;
            printf("Cannot find hostname in DNS: %s\n", scan_addr);
            continue;
        }

        for (int j = 0; j <= NUMBER_OF_PORTS; j++) {
            strcpy(addr[i][j].hostname, host->h_name);
            strcpy(addr[i][j].address, scan_addr);
            addr[i][j].port = j;
            addr[i][j].open = PORT_CLOSE;
            // printf("name: %s, ip: %s, port: %d\n", addr[i][j].hostname, addr[i][j].address, addr[i][j].port);
        }
        printf("Parse Host {name: %s, ip: %s} Done.\n", addr[i][0].hostname, addr[i][0].address);
    }

    return addr;
}

void sockconnect(struct Address* b)
{
    assert(b != NULL);
    // printf("ip:%s, port:%d\n", b->address, b->port);
    struct sockaddr_in t_addr;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return;
    }
    t_addr.sin_family = AF_INET;
    t_addr.sin_addr.s_addr = inet_addr(b->address);
    t_addr.sin_port = htons(b->port);

    if (connect(sock, (struct sockaddr*)&t_addr, sizeof(t_addr)) < 0)
        b->open = PORT_CLOSE;
    else
        b->open = PORT_OPEN;
}

void* pth_main(void* arg)
{
    Address* addr = (Address*)arg;
    for (int j = 0; j <= NUMBER_OF_PORTS; j++) {
        if (addr[j].port == -1)
            break;
        sockconnect(&addr[j]);
    }

    return NULL;
}

int main(int argc, char** argv)
{
    char addr_str[100] = "127.0.0.1";

    if (argv && argv[1]) {
        strcpy(addr_str, argv[1]);
    }

    Address** addr = TranAddr(addr_str);

    std::vector<pthread_t> pth_tid_vec;
    for (int i = 1; i <= NUMBER_OF_LAN; i++) {
        pthread_t pth_tid;
        pthread_create(&pth_tid, NULL, pth_main, addr[i]);
        pth_tid_vec.push_back(pth_tid);
    }

    for (auto mem : pth_tid_vec)
        pthread_join(mem, NULL);

    printf("主机名\t\tip地址\t\t开放端口\n");
    for (int i = 1; i <= NUMBER_OF_LAN; i++) {
        std::vector<int> portList;

        for (int j = 0; j <= NUMBER_OF_PORTS; j++) {
            if (j == 0 && addr[i][j].port == -1) {
                break;
            } else {
                if (addr[i][j].open == PORT_OPEN)
                    portList.push_back(addr[i][j].port);
            }
        }

        char portListStr[500] = "";
        if (portList.empty())
            strcpy(portListStr, "none");

        for (auto mem : portList) {
            char portStr[10] = { 0 };
            snprintf(portStr, sizeof(portStr), "\'%d\'", mem);
            strcat(portListStr, portStr);
        }

        printf("%s\t%s\t%s\n", addr[i][0].hostname, addr[i][0].address, portListStr);
    }
    return 0;
}