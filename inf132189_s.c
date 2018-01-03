#include "inf132189_h.h"

char users[20][8];
char groups[10][8];

int num_of_users = 0,
    num_of_groups = 0;

int msgid_s, //server 1234
    msgid_c; //client 4321

void init() {
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

client_msg cm;
server_msg sm;

int main() {
    init();

    //deleting message queues
    msgid_s = msgget(SERVER, IPC_CREAT|0644); 
    msgid_c = msgget(CLIENT, IPC_CREAT|0644);
    msgctl(msgid_s,IPC_RMID,NULL);
    msgctl(msgid_c,IPC_RMID,NULL);

    msgid_s = msgget(SERVER, IPC_CREAT|0644); 
    msgid_c = msgget(CLIENT, IPC_CREAT|0644);

    msgrcv(msgid_s, &cm, sizeof(cm)-sizeof(long), 1, 0);
    printf("%s\n",cm.text);

    while(1);
}