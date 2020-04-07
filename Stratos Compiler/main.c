#include <stdio.h>
#include <stdlib.h>
#include "path.c"

void getOS(void) ;
int main(void)
{


char *homedir = getenv("HOME");

        if (homedir != NULL) {
                printf("Home dir in enviroment");
                printf("%s\n", homedir);
        }


   //get os
    // getOs();



   //read file
   //readFile();

    return 0;
}

void getIfOsIs64Or32bit(){
  switch(sizeof(void*)){
        case 4: printf("32\n");
        break;
        case 8: printf("64\n");
        break;
    }

}

void getOs(){
    #ifdef _WIN32 // Includes both 32 bit and 64 bit
	    #ifdef _WIN64
	        printf("Windows 64 bit\n");
	    #else
	        printf("Windows 32 bit\n");
	    #endif
	#elif __linux__
	    printf("Linux\n");
	#elif __unix__
	    printf("Other unix OS\n");
    #elif __APPLE__
	     printf("apple os");
    #elif BSD
         printf(" bsd os ")
	#else
	    printf("Unidentified OS\n");

	#endif

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



