#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char var[20];
int selection;
void tcity()
{
    printf("\n\rt city yall");
}

void ucity()
{
    printf("\n\ru city yall");
}

void dcity()
{
    printf("\n\rd city yall");
}

void main(){
    
    printf("\n\rEnter your name: \n");
    scanf("%s", var);  // Note: %s is for char arrays and not %f(floats)
    printf("\n\rlet me look for %s", var);  // Note: no ampersand here
    const char *inputsel[]={"t","u","d"};
    const int len = sizeof(inputsel)/sizeof(inputsel[0]);
       if (strcmp(var,inputsel[0]) == 0)
       {
          tcity();
          
       }
       else if (strcmp(var,inputsel[1]) == 0)
        {
          ucity();
          
       }
       else if (strcmp(var,inputsel[2]) == 0)
        {
          dcity();
          
       }
       else
       {
           printf("\n\rhmm couldnt find what you were looking for...why dont you actually try this time?");
       }
    main();
}
