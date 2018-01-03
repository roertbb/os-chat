#include "inf132189_h.h"

int msgid_server,
    msgid_client,
    msgid_report; 

client_msg cm;
server_msg sm;
report_msg rm;

int login() {
    char username[8];
    printf("Enter username: ");
    scanf("%s", username);
    cm.type = 1;
    strcpy(cm.receiver, username);
    cm.pid = getpid();
    msgsnd(msgid_client, &cm, sizeof(cm)-sizeof(long), 0);
    msgrcv(msgid_report, &rm, sizeof(rm)-sizeof(long), getpid(), 0);
    if (rm.feedback == 0) {
        printf("Incorrect username\n");
        login();
    }
    else if (rm.feedback == 1) {
        printf("Login Successful, welcome to chat\n");
    }
    else if (rm.feedback == 2) {
        printf("User already logged in\n");
        login();
    }
    else if (rm.feedback == 3) {
        printf("Reached unsuccessful login limit\n");
        exit(0);
    }
}

int main() {
    msgid_server = msgget(SERVER, IPC_CREAT|0644);
    msgid_client = msgget(CLIENT, IPC_CREAT|0644);
    msgid_report = msgget(REPORT, IPC_CREAT|0644);

    login();
    
    // cm.type = 1;
    // strcpy(cm.text,"Hello");
    // msgsnd(msgid_server, &cm, sizeof(cm)-sizeof(long),0);

    while(1);
}