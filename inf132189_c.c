#include "inf132189_h.h"

int login(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, int pids[2]) {
    char username[8];
    printf("Enter username: ");
    scanf("%s", username);
    cm->type = 1;
    strcpy(cm->receiver, username);
    cm->pids[0] = pids[0];
    cm->pids[1] = pids[1];
    msgsnd(msgid_client, cm, sizeof(client_msg)-sizeof(long), 0);
    msgrcv(msgid_report, rm, sizeof(report_msg)-sizeof(long), pids[0], 0);
    if (rm->feedback == 0) {
        printf("Incorrect username\n");
        login(msgid_client, msgid_report, cm, rm, pids);
    }
    else if (rm->feedback == 1) {
        printf("Login Successful, welcome to chat\n");
        return 0;
    }
    else if (rm->feedback == 2) {
        printf("User already logged in\n");
        login(msgid_client, msgid_report, cm, rm, pids);
    }
    else if (rm->feedback == 3) {
        printf("Reached unsuccessful login limit\n");
        return 1;
    }
}

int logout(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, int pids[2]) {
    cm->type = 2;
    cm->pids[0] = pids[0];
    msgsnd(msgid_client, cm, sizeof(client_msg)-sizeof(long), 0);
    msgrcv(msgid_report, rm, sizeof(report_msg)-sizeof(long), getpid(), 0);
    if (rm->feedback == 1) {
        printf("Successfully logged out\n");
        return 1;
    } 
    else {
        printf("Unsuccessfully logged out\n");
    }
}

int request_user_list(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, int pids[2]) {
    cm->type = 3;
    cm->pids[0] = pids[0];
    cm->pids[1] = pids[1];
    msgsnd(msgid_client, cm, sizeof(client_msg)-sizeof(long), 0);
    msgrcv(msgid_report, rm, sizeof(report_msg)-sizeof(long), getpid(), 0);
    if (rm->feedback == 0) 
        printf("couldn't get user list from server\n");
}

int receive_user_list(server_msg * sm) {
    printf("user list:\n%s",sm->text);
}

int send_user_message(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, int pids[2]) {
    char username[8];
    char message[2048];
    printf("enter username: ");
    scanf("%s", username);
    printf("enter message: ");
    scanf("%s", message);

    cm->type = 9;
    strcpy(cm->receiver,username);
    strcpy(cm->text,message);
    cm->pids[0] = pids[0];
    cm->pids[1] = pids[1];
    msgsnd(msgid_client, cm, sizeof(client_msg)-sizeof(long), 0);
    msgrcv(msgid_report, rm, sizeof(report_msg)-sizeof(long), getpid(), 0);
    if (rm->feedback == 0)
        printf("no user found with such username\n");
}

int receive_user_message(server_msg * sm) {
    printf("[%s]: %s\n", sm->sender, sm->text);
}

int request_group_enrollment(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, int pids[2]) {
    char groupname[8];
    printf("enter groupname: ");
    scanf("%s", groupname);

    strcpy(cm->receiver, groupname);
    cm->pids[0] = pids[0];
    cm->pids[1] = pids[1];

    msgsnd(msgid_client, cm, sizeof(client_msg)-sizeof(long), 0);
    msgrcv(msgid_report, rm, sizeof(report_msg)-sizeof(long), pids[0], 0);

    if (rm->feedback == 0)
        printf("Group %s doesn't exist\n", groupname);
    else if (rm->feedback == 1)
        printf("Successful enrolled to group %s\n", groupname);
    else if (rm->feedback == 2)
        printf("User already enrolled\n");
}

int main() {
    int pids[2];

    int msgid_server,
        msgid_client,
        msgid_report; 

    client_msg cm;
    server_msg sm;
    report_msg rm;

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
                    receive_user_list(&sm);
                    break;
                case 9:
                    receive_user_message(&sm);
                    break;
            }
        }
    }
    else {
        //sending messages
        int n = login(msgid_client, msgid_report, &cm, &rm, pids);

        int options;

        while(n == 0) {
            scanf("%d", &options);
            switch(options) {
                case 2:
                    n = logout(msgid_client, msgid_report, &cm, &rm, pids);
                    break;
                case 3:
                    request_user_list(msgid_client, msgid_report, &cm, &rm, pids);
                    break;
                case 9:
                    send_user_message(msgid_client, msgid_report, &cm, &rm, pids);
                    break;
            }
        }

        kill(pids[1], SIGKILL);
    }

    printf("kthxbye\n");
}