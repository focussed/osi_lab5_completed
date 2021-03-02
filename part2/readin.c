#include <stdio.h>
#include <stdlib.h>

#define MYFILE "/dev/memory"

char num[6];

int main()
{
   FILE *fptr;

   if ((fptr = fopen(MYFILE,"r")) == NULL){
       printf("Error! opening file %s\n", MYFILE);

       // Program exits if the file pointer returns NULL.
       exit(1);
   }

   fscanf(fptr,"%s", num);

   printf("Value of str=%s", num);
   fclose(fptr); 
  
   return 0;
}

