#include "define.h"

#include <map>
#include <string>

using namespace std;
map<string, int> ip_status;

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
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(host);
    
    // 绑定套接字到多播组地址
    if (bind(fd, (struct sockaddr*)&server, sizeof(server)) < 0)
        perror("bind");
    
    return fd;
}

void* check(void* arg) {
    
    while(1) {
        printf("check===================================check\n");
        
        int cnt = 0;
        for (auto it = ip_status.begin(); it != ip_status.end(); it++) {
            if (it->second == INACTIVE) {
                it->second = KILLED;
                printf("%s killed\n", it->first.c_str());
                cnt++;
            }
            else if (it->second == ACTIVE) {
                printf("%s active\n", it->first.c_str());
                it->second = INACTIVE; 
                cnt++;
            }
        }
        if (cnt == 0)
            puts("No activities");
        printf("check===================================check\n\n");
        sleep(7);
    }
}

void run_server(const char* ip_address, int port)
{
    int server = create_server(ip_address, port);
    
    pthread_t check_tid;
    pthread_create(&check_tid, NULL, check, &server);
    
    struct sockaddr_in cliaddr;
    // 设置目标多播组地址
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
    cliaddr.sin_port = htons(MULTICAST_PORT);
    
    int recv_counter = 0;
    int send_counter = 0;
    
    while (1) {
        // 将数据发送到多播组
        char send_buff[] = "Hello, client!";
        if (sendto(server, send_buff, strlen(send_buff), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)) < 0) {
            perror("sendto");
            break;
        }
        printf("sendarrd:%s, port:%d\n", MULTICAST_GROUP, MULTICAST_PORT);
        printf("The message send is ***%s*** %d\n\n", send_buff, send_counter++);

        struct sockaddr_in client_addr;
        char recv_buff[256] = {0};
        socklen_t client_addr_len = sizeof(client_addr);
        
        // 接收多播数据并获取源地址信息
        if (recvfrom(server, recv_buff, sizeof(recv_buff), MSG_DONTWAIT, (struct sockaddr*)&client_addr, &client_addr_len) > 0) { 
            printf("Received from addr: %s, port: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            printf("The received message is ***%s*** %d\n\n", recv_buff, recv_counter++);
            
            string addr = string(inet_ntoa(client_addr.sin_addr));
            string port = to_string(ntohs(client_addr.sin_port));
            string ip = addr + ":";
            ip += port;
            
            if (ip_status.count(addr) == 0 || ip_status[addr] == KILLED) {
                puts("------------------------------------------------");
                printf("******Welcome %s join group******\n", addr.c_str());
                puts("------------------------------------------------");
            }
            ip_status[addr] = ACTIVE;
        }
        puts("");
        
        sleep(SEND_INTERVAL);
    }

    close(server);
}

int main(int argc, char* argv[])
{
    run_server(SERVER_HOST, SERVER_PORT);
    return 0;
}