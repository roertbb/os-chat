#include "inf132189_h.h"

// 0
void load_config(user * users, group * groups, int * num_of_users, int * num_of_groups) {
    int i, j;
    char buf[8];
    FILE *f;
    f = fopen("config", "r");
    fscanf(f, "%s", buf);
    if (strcmp(buf,"USERS\n")) {
        fscanf(f, "%s", buf);
        while(strcmp(buf,"GROUPS")) {
            users[*num_of_users].pids[0] = 0;
            users[*num_of_users].pids[1] = 0;
            strcpy(users[(*num_of_users)++].username,buf);
            fscanf(f, "%s", buf);
        }
        fscanf(f, "%s", buf);
        while(strcmp(buf,"END")) {
            for (i=0; i<20; i++) {
                groups[*num_of_groups].users[i]=0;
            }
            strcpy(groups[(*num_of_groups)++].groupname, buf);
            fscanf(f, "%s", buf);
        }
    }
    else {
        printf("illegal format of config file");
        exit(0);
    }
    fclose(f);

    for (i=0; i<*num_of_users; i++) {
        users[i].pids[0] = 0;
        users[i].pids[1] = 1;
        for (j=0; j<*num_of_users; j++) {
            users[i].blocked_users[j] = 0;
        }
        for (j=0; j<*num_of_groups; j++) {
            users[i].blocked_groups[j] = 0;
        }
    }

    for (i=0; i<*num_of_groups; i++) {
        for (j=0; j<*num_of_users; j++) {
            groups[i].users[j] = 0;
        }
    }
}

// 1
void handle_login(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, user * users, int num_of_users) {
    int i;
    rm->type = cm->pids[0];
    for (i=0; i<num_of_users; i++) {
        if (strcmp(cm->receiver,users[i].username) == 0) {
            if (users[i].pids[0] > 0) {
                //user already logged in
                rm->feedback = 2;
                printf("attempt to login as %s, but already logged in\n", users[i].username);
                msgsnd(msgid_report, rm, sizeof(report_msg)-sizeof(long), 0);
                return;
            }
            else {
                //user login
                rm->feedback = 1;
                users[i].pids[0] = cm->pids[0];
                users[i].pids[1] = cm->pids[1];
                printf("user %s logged in\n", users[i].username);
                msgsnd(msgid_report, rm, sizeof(report_msg)-sizeof(long), 0);
                return;
            }
        } 
    }
    //no user with such username
    rm->feedback = 0;
    printf("unsuccessful login by %d\n", cm->pids[0]);
    msgsnd(msgid_report, rm, sizeof(report_msg)-sizeof(long), 0);    
}

// 2
void handle_logout(int msgid_client, int msgid_report, client_msg * cm, report_msg * rm, user * users, int num_of_users) {
    int i;
    rm->type = cm->pids[0];
    for (i=0; i<num_of_users; i++) {
        if (cm->pids[0] == users[i].pids[0]) {
            rm->feedback = 1;
            users[i].pids[0] = 0;
            users[i].pids[1] = 0;
            printf("user %s logged out\n", users[i].username);
            msgsnd(msgid_report, rm, sizeof(report_msg)-sizeof(long), 0);
            return;
        }
    }
    //couldn't find user with received pid
    rm->feedback = 0;
    printf("user %s couldn't log out\n", users[i].username);
    msgsnd(msgid_report, rm, sizeof(report_msg)-sizeof(long), 0);
}

// 3
void handle_request_user_list(int msgid_server, int msgid_report, client_msg * cm, report_msg * rm, server_msg * sm, user * users, int num_of_users) {
    char username[8];
    int i;

    char ul[2048] = "";
    
    for (i=0; i<num_of_users; i++) {
        if (users[i].pids[0] > 0) {
            char userid[4] = "x. ";
            userid[0] = i + '0' + 1;
            strcat(ul, userid);
            strcat(ul, users[i].username);
            strcat(ul, "\n");
        }
    }

    sm->type = cm->pids[1];
    sm->msg_type = 3;
    strcpy(sm->text, ul);
    msgsnd(msgid_server, sm, sizeof(server_msg)-sizeof(long), 0);

    for (i=0; i<num_of_users; i++) {
        if (cm->pids[0] == users[i].pids[0]) {
            strcpy(username, users[i].username);
        }
    }
    printf("user %s requested user list\n", username);
    // rm->type = cm->pids[0];
    // rm->feedback = 1;
    // msgsnd(msgid_report, rm, sizeof(report_msg)-sizeof(long), 0);
}

// 4
void handle_request_group_list(int msgid_server, int msgid_report, client_msg * cm, report_msg * rm, server_msg * sm, user * users, group * groups, int num_of_users, int num_of_groups) {
    char groupname[8];
    char gl[2048] = "";
    int i;

    for (i=0; i<num_of_groups; i++) {
        char groupid[4] = "x. ";
        groupid[0] = i + '0' + 1;
        strcat(gl, groupid);
        strcat(gl, groups[i].groupname);
        strcat(gl, "\n");
    }

    sm->type = cm->pids[1];
    sm->msg_type = 4;
    strcpy(sm->text, gl);
    msgsnd(msgid_server, sm, sizeof(server_msg)-sizeof(long), 0);

    for (i=0; i<num_of_users; i++) {
        if (cm->pids[0] == users[i].pids[0])
            strcpy(groupname, users[i].username);
    }
    printf("user %s requested group list\n", groupname);
    // rm->type = cm->pids[0];
    // rm->feedback = 1;
    // msgsnd(msgid_report, rm, sizeof(report_msg)-sizeof(long), 0);
}

// 5
void handle_request_group_member_list(int msgid_server, int msgid_report, client_msg * cm, report_msg * rm, server_msg * sm, user * users, int num_of_users, group * groups, int num_of_groups) {
    int i, j;
    char username[8];
    char gl[2048] = "";

    rm->type = cm->pids[0];
    for (j=0; j<num_of_users; j++) {
        if (cm->pids[0] == users[j].pids[0]) {
            strcpy(username, users[j].username);
        }
    }

    for (i=0; i<num_of_groups; i++) {
        if (strcmp(cm->receiver, groups[i].groupname) == 0) {
            for (j=0; j<num_of_users; j++) {
                if (groups[i].users[j] == 1) {
                    char userid[4] = "x. ";
                    userid[0] = j + '0' + 1;
                    strcat(gl, userid);
                    strcat(gl, users[j].username);
                    strcat(gl, "\n");
                }
            }

            sm->type = cm->pids[1];
            sm->msg_type = 5;
            strcpy(sm->sender, cm->receiver);
            strcpy(sm->text, gl);
            msgsnd(msgid_server, sm, sizeof(server_msg)-sizeof(long), 0);

            rm->feedback = 1;
            printf("user %s requested member list of group %s\n", username, cm->receiver);
            msgsnd(msgid_report, rm, sizeof(report_msg)-sizeof(long), 0);
        }
    }
    //group doesn't exists
    rm->feedback = 0;
    printf("user %s tried to get group user list of not existing group\n", username);
    msgsnd(msgid_report, rm, sizeof(report_msg)-sizeof(long), 0);
}

// 6
void handle_request_group_enrollemnt(int msgid_report, client_msg * cm, report_msg * rm, user * users, group * groups, int num_of_users, int num_of_groups) {
    int i, j;
    rm->type = cm->pids[0];
    for (i=0; i<num_of_users; i++) {
        if (users[i].pids[0] == cm->pids[0]) {
            for (j=0; j<num_of_groups; j++) {
                if (strcmp(groups[j].groupname,cm->receiver) == 0) {
                    if (groups[j].users[i] == 0) {
                        //success
                        rm->feedback = 1;
                        groups[j].users[i] = 1;
                        printf("user %s has been enrolled into group %s\n", users[i].username, groups[j].groupname);
                    }
                    else {
                        //already in group
                        rm->feedback = 2;
                        printf("user %s already enrolled into group %s\n", users[i].username, groups[j].groupname);
                    }
                    msgsnd(msgid_report, rm, sizeof(report_msg) - sizeof(long), 0);
                    return;
                }
            }
        }
    }
    //group doesn't exists
    rm->feedback = 0;
    for (i=0; i<num_of_users; i++) {
        if (users[i].pids[0] == cm->pids[0]) {
            rm->type = users[i].pids[0];
            printf("user %s tried to enroll into non-existing group %s\n", users[i].username, cm->receiver);
            msgsnd(msgid_report, rm, sizeof(report_msg) - sizeof(long), 0);
        }
    }
}

// 7
void handle_request_group_sign_out(int msgid_report, client_msg * cm, report_msg * rm, user * users, group * groups, int num_of_users, int num_of_groups) {
    int i, j;
    for (i=0; i<num_of_users; i++) {
        if (users[i].pids[0] == cm->pids[0]) {
            for (j=0; j<num_of_groups; j++) {
                if (strcmp(groups[j].groupname, cm->receiver) == 0) {
                    rm->type = users[i].pids[0];
                    if (groups[j].users[i] == 1) {
                        //success
                        rm->feedback = 1;
                        groups[j].users[i] = 0;
                        printf("user %s has signed out from group %s\n", users[i].username, groups[j].groupname);
                    }
                    else {
                        //not enrolled
                        rm->feedback = 2;
                        printf("user %s is not enrolled into group %s\n", users[i].username, groups[j].groupname);
                    }
                    msgsnd(msgid_report, rm, sizeof(report_msg)-sizeof(long), 0);
                    return;
                }
            }
        }
    }
    //group doesn't exists
    rm->feedback = 0;
    for (i=0; i<num_of_users; i++) {
        if (users[i].pids[0] == cm->pids[0]) {
            rm->type = users[i].pids[0];
            printf("user %s tried to sign out from group %s\n", users[i].username, cm->receiver);
            msgsnd(msgid_report, rm, sizeof(report_msg)-sizeof(long), 0);
        }
    }
}

// 8
void handle_user_message(int msgid_server, int msgid_report, client_msg * cm, report_msg * rm, server_msg * sm, user * users, int num_of_users) {
    int i, j;
    char username[8];
    for (j=0; j<num_of_users; j++) {
        if (users[j].pids[0] == cm->pids[0]) {
            strcpy(username, users[j].username);
        }
    }

    for (i=0; i<num_of_users; i++) {
        if (strcmp(cm->receiver,users[i].username) == 0) {
            // find sender pid
            int userid;

            for (j=0; j<num_of_users; j++) {
                if (users[j].pids[0] == cm->pids[0])
                    userid = j;
            }

            //send message to user
            sm->type = users[i].pids[1];

            rm->type = cm->pids[0];
            if (users[i].blocked_users[userid] == 0) {
                // send
                sm->msg_type = 8;
                strcpy(sm->sender, username);
                strcpy(sm->text, cm->text);
                msgsnd(msgid_server, sm, sizeof(server_msg)-sizeof(long), 0);

                //confirm to client
                rm->feedback = 1;
                printf("user %s send message to user %s\n", username, cm->receiver);
            }
            else {
                // blocked
                rm->feedback = 2;
                printf("user %s send message to user %s, but blocked\n", username, cm->receiver);
            }

            msgsnd(msgid_report, rm, sizeof(report_msg)-sizeof(long), 0);
            return;
        }
    }
    //wrong username
    rm->type = cm->pids[0];
    rm->feedback = 0;
    printf("user %s send message, but with wrong username\n", username);
    msgsnd(msgid_report, rm, sizeof(report_msg)-sizeof(long), 0);
}

// 9
void handle_group_message(int msgid_server, int msgid_report, client_msg * cm, report_msg * rm, server_msg * sm, user * users, group * groups, int num_of_users, int num_of_groups) {
    int i, j, sender_id;
    char groupname[16], username[8];

    // find sender of the message (user)
    for (j=0; j<num_of_users; j++) {
        if (cm->pids[0] == users[j].pids[0]) {
            strcpy(username, users[j].username);
            sender_id = j;
        }
    }

    for (i=0; i<num_of_groups; i++) {
        if (strcmp(cm->receiver, groups[i].groupname) == 0) {
            strcpy(groupname, groups[i].groupname);
            strcat(groupname, ":");
            strcat(groupname, username);
            strcpy(sm->sender,groupname);
            strcpy(sm->text, cm->text);
            sm->msg_type = 9;
            
            for (j=0; j<num_of_users; j++) {
                if (groups[i].users[j] == 1) {
                    if (users[j].blocked_groups[i] == 0 && users[j].blocked_users[sender_id] == 0) {
                        sm->type = users[j].pids[1];
                        msgsnd(msgid_server, sm, sizeof(server_msg)-sizeof(long), 0);
                    }
                }
            }

            rm->type = cm->pids[0];
            rm->feedback = 1;
            printf("user %s send message to group %s\n", username, cm->receiver);
            msgsnd(msgid_report, rm, sizeof(report_msg)-sizeof(long), 0);
            return;
        }
    }
    //group doesn't exists
    rm->type = cm->pids[0];
    rm->feedback = 0;
    printf("user %s send message, but with wrong groupname\n", username);
    msgsnd(msgid_report, rm, sizeof(report_msg)-sizeof(long), 0); 
}

// 10
void handle_request_user_block(int msgid_report, client_msg * cm, report_msg * rm, server_msg * sm, user * users, int num_of_users) {
    int i, j;
    rm->type = cm->pids[0];
    for (i=0; i<num_of_users; i++) {
        if (strcmp(cm->receiver,users[i].username) == 0) {
            for (j=0; j<num_of_users; j++) {
                if (users[j].pids[0] == cm->pids[0]) {
                    if (users[j].blocked_users[i] == 1) {
                        // already blocked
                        rm->feedback = 2;
                        printf("user %s requested to block user %s, but %s is already blocked\n", users[j].username, cm->receiver, cm->receiver);
                    }
                    else {
                        // block
                        users[j].blocked_users[i] = 1;
                        rm->feedback = 1;
                        printf("user %s blocked user %s\n", users[j].username, cm->receiver);
                    }
                    msgsnd(msgid_report, rm, sizeof(report_msg)-sizeof(long), 0);
                    return;
                }
            }
        }
    }
    rm->feedback = 0;
    for (i=0; i<num_of_users; i++) {
        if (users[i].pids[0] == cm->pids[0]) {
            printf("unsuccessful block request by %s, user not found\n", users[i].username);
            msgsnd(msgid_report, rm, sizeof(report_msg)-sizeof(long), 0);
            return;
        }
    }
}

// 11
void handle_request_group_block(int msgid_report, client_msg * cm, report_msg * rm, server_msg * sm, user * users, group * groups, int num_of_users, int num_of_groups) {
    int i, j;

    for (i=0; i<num_of_groups; i++) {
        if (strcmp(cm->receiver, groups[i].groupname) == 0) {
            for (j=0; j<num_of_users; j++) {
                if (users[j].pids[0] == cm->pids[0]) {
                    if (users[j].blocked_groups[i] == 1) {
                        // already blocked
                        rm->feedback = 2;
                        printf("user %s requested to block group %s, but %s is already blocked\n", users[j].username, cm->receiver, cm->receiver);
                    }
                    else {
                        //block
                        users[j].blocked_groups[i] = 1;
                        rm->feedback = 1;
                        printf("user %s blocked group %s\n", users[j].username, cm->receiver);
                    }
                    msgsnd(msgid_report, rm, sizeof(report_msg)-sizeof(long), 0);
                    return;
                }
            }
        }
    }
    rm->feedback = 0;
    for (i=0; i<num_of_users; i++) {
        if (users[i].pids[0] == cm->pids[0]) {
            printf("unsuccessful block request by %s, group not found\n", users[i].username);
            msgsnd(msgid_report, rm, sizeof(report_msg)-sizeof(long), 0);
            return;
        }
    }

}

int main(int argc, char *argv[]) {
    user * users = (user*) malloc(20*sizeof(user));
    group * groups = (group*) malloc(10*sizeof(group));

    int num_of_users = 0,
        num_of_groups = 0;

    int msgid_server,
        msgid_client,
        msgid_report;

    client_msg cm;
    server_msg sm;
    report_msg rm;

    load_config(users, groups, &num_of_users, &num_of_groups);

    //deleting message queues
    msgid_server = msgget(SERVER, IPC_CREAT|0644); 
    msgid_client = msgget(CLIENT, IPC_CREAT|0644);
    msgid_report = msgget(CLIENT, IPC_CREAT|0644);
    msgctl(msgid_server,IPC_RMID,NULL);
    msgctl(msgid_client,IPC_RMID,NULL);
    msgctl(msgid_report,IPC_RMID,NULL);

    //creating new
    msgid_server = msgget(SERVER, IPC_CREAT|0644); 
    msgid_client = msgget(CLIENT, IPC_CREAT|0644);
    msgid_report = msgget(REPORT, IPC_CREAT|0644); 

    printf("chat server is running...\n");

    while(1) {
        msgrcv(msgid_client, &cm, sizeof(cm)-sizeof(long), 0, 0);
        switch(cm.type) {
            case 1:
                handle_login(msgid_client, msgid_report, &cm, &rm, users, num_of_users);
                break;
            case 2:
                handle_logout(msgid_client, msgid_report, &cm, &rm, users, num_of_users);
                break;
            case 3:
                handle_request_user_list(msgid_server, msgid_report, &cm, &rm, &sm, users, num_of_users);
                break;
            case 4:
                handle_request_group_list(msgid_server, msgid_report, &cm, &rm, &sm, users, groups, num_of_users, num_of_groups);
                break;
            case 5:
                handle_request_group_member_list(msgid_server, msgid_report, &cm, &rm, &sm, users, num_of_users, groups, num_of_groups);
                break;
            case 6:
                handle_request_group_enrollemnt(msgid_report, &cm, &rm, users, groups, num_of_users, num_of_groups);
                break;
            case 7:
                handle_request_group_sign_out(msgid_report, &cm, &rm, users, groups, num_of_users, num_of_groups);
                break;
            case 8:
                handle_user_message(msgid_server, msgid_report, &cm, &rm, &sm, users, num_of_users);
                break;
            case 9:
                handle_group_message(msgid_server, msgid_report, &cm, &rm, &sm, users, groups, num_of_users, num_of_groups);
                break;
            case 10:
                handle_request_user_block(msgid_report, &cm, &rm, &sm, users, num_of_users);
                break;
            case 11:
                handle_request_group_block(msgid_report, &cm, &rm, &sm, users, groups, num_of_users, num_of_groups);
                break;
        }
    }
    
    free(users);
    free(groups);
}