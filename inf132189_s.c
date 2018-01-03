#include "inf132189_h.h"

char users[20][8];
char groups[10][8];

int num_of_users = 0,
    num_of_groups = 0;

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

int main() {
    init();

    
}