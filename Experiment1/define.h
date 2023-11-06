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

const char HOST[] = "127.0.0.1";
const int PORT = 1234;
const int SEND_INTERVAL = 1;
const char MULTICAST_GROUP[] = "224.0.0.2";
const int MULTICAST_PORT = 12345;