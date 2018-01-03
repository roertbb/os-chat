#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>

#define SERVER 1234
#define CLIENT 4321

typedef struct client_msg client_msg;
struct client_msg {
    long type;
    int pid;
    char receiver[8];
    char text[2048];
};

typedef struct server_msg server_msg;
struct server_msg {
    long type;
    int report;
    char sender[8];
    char text[2048];
};

#endif