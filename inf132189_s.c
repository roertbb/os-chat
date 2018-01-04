#include "inf132189_h.h"

user users[20];
group groups[10];

// char users[20][8];
// char groups[10][8];

// int user_status[20][2];

int num_of_users = 0,
    num_of_groups = 0;

int msgid_server, //server 1234
    msgid_client, //client 1235
    msgid_report; //report 1236

client_msg cm;
server_msg sm;
report_msg rm;

void load_config() {
    int i;
    char buf[8];
    FILE *f;
    f = fopen("config", "r");
    fscanf(f, "%s", buf);
    if (strcmp(buf,"USERS\n")) {
        fscanf(f, "%s", buf);
        while(strcmp(buf,"GROUPS")) {
            users[num_of_users].pids[0] = 0;
            users[num_of_users].pids[1] = 0;
            strcpy(users[num_of_users++].username,buf);
            fscanf(f, "%s", buf);
        }
        fscanf(f, "%s", buf);
        while(strcmp(buf,"END")) {
            for (i=0; i<20; i++) {
                groups[num_of_groups].users[i]=0;
            }
            strcpy(groups[num_of_groups++].groupname, buf);
            fscanf(f, "%s", buf);
        }
    }
    else {
        printf("illegal format of config file");
        exit(0);
    }
    fclose(f);
    
    // printf("%d\n", num_of_users);
    // for (i=0; i<num_of_users; i++) {
    //     printf("%s",users[i].username);
    // }
    // printf("\n%d\n", num_of_groups);
    // for (i=0; i<num_of_groups; i++) {
    //     printf("%s",groups[i].groupname);
    // }
}

void handle_login() {
    int i;
    for (i=0; i<num_of_users; i++) {
        if (strcmp(cm.receiver,users[i].username) == 0) {
            if (users[i].pids[0] > 0) {
                //already logged in
                rm.type = cm.pids[0];
                rm.feedback = 2;
                printf("attempt to login as %s, but already logged in\n", users[i].username);
                //[TODO]check how many times pid tried to login
                msgsnd(msgid_report, &rm, sizeof(rm)-sizeof(long), 0);
                return;
            }
            else {
                //login
                rm.type = cm.pids[0];
                rm.feedback = 1;
                printf("user %s logged in\n", users[i].username);
                msgsnd(msgid_report, &rm, sizeof(rm)-sizeof(long), 0);
                users[i].pids[0] = cm.pids[0];
                users[i].pids[1] = cm.pids[1];
                return;
            }
        } 
    }
    //no user with such username
    rm.type = cm.pids[0];
    rm.feedback = 0;
    printf("unsuccessful login by %d\n", cm.pids[0]);
    //[TODO]check how many times pid tried to login
    msgsnd(msgid_report, &rm, sizeof(rm)-sizeof(long), 0);    
}

void handle_logout() {
    int i;
    for (i=0; i<num_of_users; i++) {
        if (strcmp(cm.receiver,users[i].username) == 0) {
            rm.type = cm.pids[0];
            rm.feedback = 1;
            users[i].pids[0] = 0;
            users[i].pids[0] = 0;
            printf("user %s logged out\n", users[i].username);
            msgsnd(msgid_report, &rm, sizeof(rm)-sizeof(long), 0);
            return;
        }
    }
    //something went wrong
    rm.type = cm.pids[0];
    rm.feedback = 0;
    printf("user %s couldn't log out\n", users[i].username);
    msgsnd(msgid_report, &rm, sizeof(rm)-sizeof(long), 0);
}

void handle_request_user_list() {
    char username[8];
    int i;

    rm.type = cm.pids[0];
    rm.feedback = 1;
    for (i=0; i<20; i++) {
        if (cm.pids[0] == users[i].pids[0]) {
            strcpy(username, users[i].username);
        }
    }
    printf("user %s requested user list\n", username);
    msgsnd(msgid_report, &rm, sizeof(rm)-sizeof(long), 0);

    char ul[2048] = "";
    
    for (i=0; i<num_of_users; i++) {
        if (users[i].pids[0] > 0) {
            char user[20] = "";
            char userid[3] = "x. ";
            userid[0] = i + '0' + 1;
            strcat(ul, userid);
            strcat(ul, users[i].username);
            strcat(ul, "\n");
        }
    }

    sm.type = cm.pids[1];
    sm.msg_type = 3;
    strcpy(sm.text, ul);
    msgsnd(msgid_server, &sm, sizeof(sm)-sizeof(long), 0);
}

int main() {
    load_config();

    //deleting message queues
    msgid_server = msgget(SERVER, IPC_CREAT|0644); 
    msgid_client = msgget(CLIENT, IPC_CREAT|0644);
    msgid_report = msgget(CLIENT, IPC_CREAT|0644);
    msgctl(msgid_server,IPC_RMID,NULL);
    msgctl(msgid_client,IPC_RMID,NULL);
    msgctl(msgid_report,IPC_RMID,NULL);

    //creatin new
    msgid_server = msgget(SERVER, IPC_CREAT|0644); 
    msgid_client = msgget(CLIENT, IPC_CREAT|0644);
    msgid_report = msgget(REPORT, IPC_CREAT|0644); 

    printf("chat server\n");

    while(1) {
        msgrcv(msgid_client, &cm, sizeof(cm)-sizeof(long), 0, 0);
        switch(cm.type) {
            case 1:
                handle_login();
                break;
            case 2:
                handle_logout();
                break;
            case 3:
                handle_request_user_list();
                break;
        }
    }
    msgrcv(msgid_server, &cm, sizeof(cm)-sizeof(long), 1, 0);
    printf("%s\n",cm.text);

    while(1);
}