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
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define SERVER 1234
#define CLIENT 1235
#define REPORT 1236

typedef struct client_msg client_msg;
struct client_msg {
    long type;
    int pids[2];
    char receiver[8];
    char text[2048];
};

typedef struct server_msg server_msg;
struct server_msg {
    long type;
    int msg_type;
    char sender[16];
    char text[2048];
};

typedef struct report_msg report_msg;
struct report_msg {
    long type;
    int feedback;
};

typedef struct user user;
struct user {
    char username[8];
    int pids[2];
    int blocked_users[20];
    int blocked_groups[20];
};

typedef struct group group;
struct group {
    char groupname[8];
    int users[20];
};

#endif