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

struct client_msg {
    long typ;
    int pid;
    char odbiorca[8];
    char tresc[2048];
};

struct server_msg {
    long typ;
    int raport;
    char nadawca[8];
    char tresc[2048];
};

#endif