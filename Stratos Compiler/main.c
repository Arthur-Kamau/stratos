#include <stdio.h>
#include <stdlib.h>


int main()
{
   //read file
   readFile();

    return 0;
}

void readFile(){


     char path[] = {"/home/publisher/Development/stratos/src/main.st"};

     char data[6000];
     FILE *fptr ;


     fptr = fopen(path, "r");

     if(fptr == NULL){
         printf("errror");
         exit(1);
     }else{
        printf("file exist \n");
     }

   char currentline[100];

  while (fgets(currentline, sizeof(currentline), fptr) != NULL) {
        fprintf(stderr, "%s\n", currentline);
        /* Do something with `currentline` */
    }

     fclose(fptr);
}
