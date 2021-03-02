/*
 * This program asks the user for a number
 * it then opens a file for writing, writes the number to the file
 * and closes the file
 */
#include <stdio.h>
#include <stdlib.h>

#define MYFILE "/dev/memory"

int main()
{
   int num;
   FILE *fptr;

   // use appropriate location if you are using MacOS or Linux
   fptr = fopen(MYFILE,"w");

   if(fptr == NULL)
   {
      printf("Error opening the file %s\n", MYFILE);   
      exit(1);             
   }

   printf("Enter number: ");
   scanf("%d",&num);

   fprintf(fptr,"%d",num);
   fclose(fptr);

   return 0;
}
