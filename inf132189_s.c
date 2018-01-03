#include "inf132189_h.h"

char users[20][8];
char groups[10][8];

int user_status[20] = {0};

int num_of_users = 0,
    num_of_groups = 0;

int msgid_server, //server 1234
    msgid_client, //client 1235
    msgid_report; //report 1236

client_msg cm;
server_msg sm;
report_msg rm;

void load_config() {
    char buf[8];
    FILE *f;
    f = fopen("config", "r");
    fscanf(f, "%s", buf);
    if (strcmp(buf,"USERS\n")) {
        fscanf(f, "%s", buf);
        while(strcmp(buf,"GROUPS")) {
            strcpy(users[num_of_users++],buf);
            fscanf(f, "%s", buf);
        }
        fscanf(f, "%s", buf);
        while(strcmp(buf,"END")) {
            strcpy(groups[num_of_groups++],buf);
            fscanf(f, "%s", buf);
        }
    }
    else {
        printf("illegal format of config file");
        exit(0);
    }
    fclose(f);

    // int i;
    // printf("%d\n", num_of_users);
    // for (i=0; i<num_of_users; i++) {
    //     printf("%s",users[i]);
    // }
    // printf("\n%d\n", num_of_groups);
    // for (i=0; i<num_of_groups; i++) {
    //     printf("%s",groups[i]);
    // }
}

void handle_login() {
    int i;
    for (i=0; i<num_of_users; i++) {
        if (strcmp(cm.receiver,users[i]) == 0) {
            if (user_status[i] > 0) {
                //already logged in
                rm.type = cm.pid;
                rm.feedback = 2;
                printf("attempt to login as %s, but already logged in\n", users[i]);
                //[TODO]check how many times pid tried to login
                msgsnd(msgid_report, &rm, sizeof(rm)-sizeof(long), 0);
                return;
            }
            else {
                //login
                rm.type = cm.pid;
                rm.feedback = 1;
                printf("user %s logged in\n", users[i]);
                msgsnd(msgid_report, &rm, sizeof(rm)-sizeof(long), 0);
                user_status[i] = cm.pid;
                return;
            }
        } 
    }
    //no user with such username
    rm.type = cm.pid;
    rm.feedback = 0;
    printf("unsuccessful login by %d\n", cm.pid);
    //[TODO]check how many times pid tried to login
    msgsnd(msgid_report, &rm, sizeof(rm)-sizeof(long), 0);    
}

void handle_logout() {
    int i;
    for (i=0; i<num_of_users; i++) {
        if (strcmp(cm.receiver,users[i]) == 0) {
            rm.type = cm.pid;
            rm.feedback = 1;
            user_status[i] = 0;
            printf("user %s logged out\n", users[i]);
            msgsnd(msgid_report, &rm, sizeof(rm)-sizeof(long), 0);
            return;
        }
    }
    //something went wrong
    rm.type = cm.pid;
    rm.feedback = 0;
    printf("user %s couldn't log out\n", users[i]);
    msgsnd(msgid_report, &rm, sizeof(rm)-sizeof(long), 0);
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
        }
    }
    msgrcv(msgid_server, &cm, sizeof(cm)-sizeof(long), 1, 0);
    printf("%s\n",cm.text);

    while(1);
}