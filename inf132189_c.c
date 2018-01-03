#include "inf132189_h.h"

int msgid_s, //server 1234
    msgid_c; //client 4321

client_msg cm;
server_msg sm;

int main() {
    
    msgid_s = msgget(1234, IPC_CREAT|0644);
    
    cm.type = 1;
    strcpy(cm.text,"Hello");
    msgsnd(msgid_s, &cm, sizeof(cm)-sizeof(long),0);

    while(1);
}