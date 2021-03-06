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
    return 0;
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
    return 0;
}

// 3
void request_users_list(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, int pids[2]) {
    cm->type = 3;
    cm->pids[0] = pids[0];
    cm->pids[1] = pids[1];
    msgsnd(msgid_client, cm, sizeof(client_msg)-sizeof(long), 0);
    // msgrcv(msgid_report, rm, sizeof(report_msg)-sizeof(long), pids[0], 0);
    // if (rm->feedback == 0) 
    //     printf("couldn't get user list from server\n");
}

// 3
void receive_users_list(server_msg * sm) {
    printf("user list:\n%s",sm->text);
}

// 4
void request_groups_list(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, int pids[2]) {
    cm->type = 4;
    cm->pids[0] = pids[0];
    cm->pids[1] = pids[1];
    msgsnd(msgid_client, cm, sizeof(client_msg)-sizeof(long), 0);
    // msgrcv(msgid_report, rm, sizeof(report_msg)-sizeof(long), pids[0], 0);
    // if (rm->feedback == 0)
    //     printf("couldn't get group list from server\n");
}

//4
void receive_groups_list(server_msg * sm) {
    printf("group list:\n%s", sm->text);
}

// 5
void request_group_member_list(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, int pids[2]) {
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
        printf("group %s doesn't exist\n", groupname);
}

// 5
void receive_group_member_list(server_msg * sm) {
    if (strlen(sm->text) <= 3)
        printf("group %s is empty :(\n", sm->sender);
    else
        printf("member list of group %s:\n%s", sm->sender, sm->text);
}

// 6
void request_group_enrollment(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, int pids[2]) {
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
void request_group_sign_out(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, int pids[2]) {
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
void send_user_message(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, int pids[2]) {
    char username[8];
    char message[2048];
    printf("enter username: ");
    scanf("%s", username);
    getchar();
    printf("enter message: ");
    scanf("%[^\n]s", message);

    cm->type = 8;
    strcpy(cm->receiver,username);
    strcpy(cm->text,message);
    cm->pids[0] = pids[0];
    cm->pids[1] = pids[1];
    msgsnd(msgid_client, cm, sizeof(client_msg)-sizeof(long), 0);
    msgrcv(msgid_report, rm, sizeof(report_msg)-sizeof(long), getpid(), 0);
    if (rm->feedback == 0)
        printf("no user found with such username\n");
    else if (rm->feedback == 2)
        printf("user %s has blocked messages from You\n", cm->receiver);
}

// 8 & 9
void receive_message(server_msg * sm) {
    printf("[%s]: %s\n", sm->sender, sm->text);
}

// 9
void send_group_message(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, int pids[2]) {
    char groupname[8];
    char message[2048];
    printf("enter groupname: ");
    scanf("%s", groupname);
    getchar();
    printf("enter message: ");
    scanf("%[^\n]s", message);

    cm->type = 9;
    strcpy(cm->receiver, groupname);
    strcpy(cm->text, message);
    cm->pids[0] = pids[0];
    cm->pids[1] = pids[1];
    msgsnd(msgid_client, cm, sizeof(client_msg)-sizeof(long), 0);
    msgrcv(msgid_report, rm, sizeof(report_msg)-sizeof(long), pids[0], 0);
    if (rm->feedback == 0)
        printf("no group found with such groupname\n");
}

// 10
void request_user_block(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, int pids[2]) {
    char username[8];
    printf("enter username: ");
    scanf("%s", username);
    
    cm->type = 10;
    strcpy(cm->receiver, username);
    cm->pids[0] = pids[0];
    msgsnd(msgid_client, cm, sizeof(client_msg)-sizeof(long), 0);
    msgrcv(msgid_report, rm, sizeof(report_msg)-sizeof(long), pids[0], 0);
    if (rm->feedback == 0)
        printf("user %s doesn't exist\n", username);
    else if (rm->feedback == 1)
        printf("user %s has been blocked\n", username);
    else if (rm->feedback == 2)
        printf("user %s has been already blocked\n", username);
}

// 11
void request_group_block(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, int pids[2]) {
    char groupname[8];
    printf("enter groupname: ");
    scanf("%s", groupname);

    cm->type = 11;
    strcpy(cm->receiver, groupname);
    cm->pids[0] = pids[0];
    msgsnd(msgid_client, cm, sizeof(client_msg)-sizeof(long), 0);
    msgrcv(msgid_report, rm, sizeof(report_msg)-sizeof(long), pids[0], 0);
    if (rm->feedback == 0)
        printf("group %s doesn't exist\n", groupname);
    else if (rm->feedback == 1)
        printf("group %s has been blocked\n", groupname);
    else if (rm->feedback == 2)
        printf("group %s has been already blocked\n", groupname);
}

void print_options() {
    printf("choose:\n[1] list of commands\n[2] logout\n[3] list of online users\n[4] list of groups\n[5] list of group members\n[6] sign in to group\n[7] sign out from group\n[8] send message to user\n[9] send message to group\n[10] block user\n[11] block group\n");
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
                    receive_users_list(&sm);
                    break;
                case 4:
                    receive_groups_list(&sm);
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
        //sending messages and waiting for response
        int options;
        int n = login(msgid_client, msgid_report, &cm, &rm, pids);

        print_options();

        while(n == 0) {
            scanf("%d", &options);
            switch(options) {
                case 1:
                    print_options();
                    break;
                case 2:
                    n = logout(msgid_client, msgid_report, &cm, &rm, pids);
                    break;
                case 3:
                    request_users_list(msgid_client, msgid_report, &cm, &rm, pids);
                    break;
                case 4:
                    request_groups_list(msgid_client, msgid_report, &cm, &rm, pids);
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
                case 10:
                    request_user_block(msgid_client, msgid_report, &cm, &rm, pids);
                    break;
                case 11:
                    request_group_block(msgid_client, msgid_report, &cm, &rm, pids);
                    break;
            }
        }

        kill(pids[1], SIGKILL);
    }

    printf("kthxbye\n");
}