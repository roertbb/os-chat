#include "inf132189_h.h"

// 1
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

// 2
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

// 3
int request_user_list(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, int pids[2]) {
    cm->type = 3;
    cm->pids[0] = pids[0];
    cm->pids[1] = pids[1];
    msgsnd(msgid_client, cm, sizeof(client_msg)-sizeof(long), 0);
    msgrcv(msgid_report, rm, sizeof(report_msg)-sizeof(long), pids[0], 0);
    if (rm->feedback == 0) 
        printf("couldn't get user list from server\n");
}

// 3
int receive_user_list(server_msg * sm) {
    printf("user list:\n%s",sm->text);
}

// 4
int request_group_list(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, int pids[2]) {
    cm->type = 4;
    cm->pids[0] = pids[0];
    cm->pids[1] = pids[1];
    msgsnd(msgid_client, cm, sizeof(client_msg)-sizeof(long), 0);
    msgrcv(msgid_report, rm, sizeof(report_msg)-sizeof(long), pids[0], 0);
    if (rm->feedback == 0)
        printf("couldn't get group list from server\n");
}

//4
int receive_group_list(server_msg * sm) {
    printf("group list:\n%s", sm->text);
}

// 5
int request_group_member_list(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, int pids[2]) {
    char groupname[8];
    printf("enter groupname: ");
    scanf("%s", groupname);
    strcpy(cm->receiver, groupname);
    cm->type = 5;
    cm->pids[0] = pids[0];
    cm->pids[1] = pids[1];
    msgsnd(msgid_client, cm, sizeof(client_msg)-sizeof(long), 0);
    msgrcv(msgid_report, rm, sizeof(report_msg)-sizeof(long), pids[0], 0);
    if (rm->feedback == 0)
        printf("couldn't get group member list from server\n");
}

// 5
int receive_group_member_list(server_msg * sm) {
    if (strlen(sm->text) <= 3)
        printf("group %s is empty :(\n", sm->sender);
    else
        printf("member list of group %s:\n%s", sm->sender, sm->text);
}

// 6
int request_group_enrollment(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, int pids[2]) {
    char groupname[8];
    printf("enter groupname: ");
    scanf("%s", groupname);

    strcpy(cm->receiver, groupname);
    cm->type = 6;
    cm->pids[0] = pids[0];
    cm->pids[1] = pids[1];

    msgsnd(msgid_client, cm, sizeof(client_msg)-sizeof(long), 0);
    msgrcv(msgid_report, rm, sizeof(report_msg)-sizeof(long), pids[0], 0);

    if (rm->feedback == 0)
        printf("Group %s doesn't exist\n", groupname);
    else if (rm->feedback == 1)
        printf("Successfully enrolled to group %s\n", groupname);
    else if (rm->feedback == 2)
        printf("User already enrolled into group %s\n", groupname);
}

// 7
int request_group_sign_out(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, int pids[2]) {
    char groupname[8];
    printf("enter groupname: ");
    scanf("%s", groupname);

    strcpy(cm->receiver, groupname);
    cm->type = 7;
    cm->pids[0] = pids[0];
    cm->pids[1] = pids[1];

    msgsnd(msgid_client, cm, sizeof(client_msg)-sizeof(long), 0);
    msgrcv(msgid_report, rm, sizeof(report_msg)-sizeof(long), pids[0], 0);

    if (rm->feedback == 0)
        printf("Group %s doesn't exist\n", groupname);
    else if (rm->feedback == 1)
        printf("Successfully signed out from group %s\n", groupname);
    else if (rm->feedback == 2)
        printf("User is not enrolled into group %s\n", groupname);
}

// 8
int send_user_message(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, int pids[2]) {
    char username[8];
    char message[2048];
    printf("enter username: ");
    scanf("%s", username);
    printf("enter message: ");
    scanf("%s", message);

    cm->type = 8;
    strcpy(cm->receiver,username);
    strcpy(cm->text,message);
    cm->pids[0] = pids[0];
    cm->pids[1] = pids[1];
    msgsnd(msgid_client, cm, sizeof(client_msg)-sizeof(long), 0);
    msgrcv(msgid_report, rm, sizeof(report_msg)-sizeof(long), getpid(), 0);
    if (rm->feedback == 0)
        printf("no user found with such username\n");
}

// 8 & 9
int receive_message(server_msg * sm) {
    printf("[%s]: %s\n", sm->sender, sm->text);
}

// 9
int send_group_message(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, int pids[2]) {
    char groupname[8];
    char message[2048];
    printf("enter username: ");
    scanf("%s", groupname);
    printf("enter message: ");
    scanf("%s", message);

    cm->type = 9;
    strcpy(cm->receiver, groupname);
    strcpy(cm->text, message);
    cm->pids[0] = pids[0];
    cm->pids[1] = pids[1];
    msgsnd(msgid_client, cm, sizeof(client_msg)-sizeof(long), 0);
    msgrcv(msgid_report, rm, sizeof(report_msg)-sizeof(long), pids[0], 0);
    if (rm->feedback == 0)
        printf("no group found with such username\n");
}

// 10

// 11

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
                case 4:
                    receive_group_list(&sm);
                    break;
                case 5:
                    receive_group_member_list(&sm);
                    break;
                case 8:
                case 9:
                    receive_message(&sm);
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
                case 4:
                    request_group_list(msgid_client, msgid_report, &cm, &rm, pids);
                    break;
                case 5:
                    request_group_member_list(msgid_client, msgid_report, &cm, &rm, pids);
                    break;
                case 6:
                    request_group_enrollment(msgid_client, msgid_report, &cm, &rm, pids);
                    break;
                case 7:
                    request_group_sign_out(msgid_client, msgid_report, &cm, &rm, pids);
                    break;
                case 8:
                    send_user_message(msgid_client, msgid_report, &cm, &rm, pids);
                    break;
                case 9:
                    send_group_message(msgid_client, msgid_report, &cm, &rm, pids);
                    break;
            }
        }

        kill(pids[1], SIGKILL);
    }

    printf("kthxbye\n");
}