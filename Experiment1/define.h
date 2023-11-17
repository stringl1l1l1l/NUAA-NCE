#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

const char SERVER_HOST[] = "0.0.0.0";
const int SERVER_PORT = 1234;
const int SEND_INTERVAL = 1;
const char MULTICAST_GROUP[] = "224.0.0.2";
const int MULTICAST_PORT = 12345;
const int TEST_CNT = 7;
const int BUFF_CAPACITY = 4;

const int KILLED = 0;
const int ACTIVE = 1;
const int INACTIVE = 2;