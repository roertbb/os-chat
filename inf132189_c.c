#include "inf132189_h.h"

int pids[2];

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
    cm.pids[0] = pids[0];
    cm.pids[1] = pids[1];
    msgsnd(msgid_client, &cm, sizeof(cm)-sizeof(long), 0);
    msgrcv(msgid_report, &rm, sizeof(rm)-sizeof(long), pids[0], 0);
    if (rm.feedback == 0) {
        printf("Incorrect username\n");
        login();
    }
    else if (rm.feedback == 1) {
        printf("Login Successful, welcome to chat\n");
        return 0;
    }
    else if (rm.feedback == 2) {
        printf("User already logged in\n");
        login();
    }
    else if (rm.feedback == 3) {
        printf("Reached unsuccessful login limit\n");
        return 1;
    }
}

int logout() {
    cm.type = 2;
    cm.pids[0] = pids[0];
    msgsnd(msgid_client, &cm, sizeof(cm)-sizeof(long), 0);
    msgrcv(msgid_report, &rm, sizeof(rm)-sizeof(long), getpid(), 0);
    if (rm.feedback == 1) {
        printf("Successfully logged out\n");
        return 1;
    } 
    else {
        printf("Unsuccessfully logged out\n");
    }
}

int request_user_list() {
    cm.type = 3;
    cm.pids[0] = pids[0];
    cm.pids[1] = pids[1];
    msgsnd(msgid_client, &cm, sizeof(cm)-sizeof(long), 0);
    msgrcv(msgid_report, &rm, sizeof(rm)-sizeof(long), getpid(), 0);
    if (rm.feedback == 0) 
        printf("couldn't get user list from server\n");
}

int receive_user_list() {
    printf("user list:\n%s",sm.text);
}

int main() {
    pids[1] = fork();
    pids[0] = getpid();

    msgid_server = msgget(SERVER, IPC_CREAT|0644);
    msgid_client = msgget(CLIENT, IPC_CREAT|0644);
    msgid_report = msgget(REPORT, IPC_CREAT|0644);

    if (pids[1] == 0) {
        // receiving messages
        while(1) {
            msgrcv(msgid_server, &sm, sizeof(sm)-sizeof(long), getpid(), 0);
            switch(sm.msg_type) {
                case 3:
                    receive_user_list();
                    break;
            }
        }
    }
    else {
        //sending messages
        int n = login();

        request_user_list();

        while(n == 0) {
            printf("loop\n");
            n = logout();
        }

        kill(pids[1], SIGKILL);
    }

    printf("kthxbye\n");
}