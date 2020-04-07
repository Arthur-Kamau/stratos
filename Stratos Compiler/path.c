#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>



void getPath(){

char *homedir = getenv("HOME");

        if (homedir != NULL) {
                printf("Home dir in enviroment");
                printf("%s\n", homedir);
        }else{
        printf("home is null");
        }
}

