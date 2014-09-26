#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


int main( )
{
    char *user_input = (char*)calloc(256, sizeof(char));
    char **user_cmd;
    char *token;
    int i = 0;
    
    fgets(user_input, 256, stdin);
    while (strcmp(user_input, "\n") == 0)
    {
        printf("Enter string: ");
        fgets(user_input, 256, stdin);
    }
    user_input = strtok(user_input, "\n"); //removes \n at end of string
    user_cmd = (char**)calloc(strlen(user_input)*2, sizeof(char));
    token = strtok(user_input, " ");
    
    while (token != NULL)
    {
        user_cmd[i] = (char*)calloc(strlen(token), sizeof(char));
        strcpy(user_cmd[i], token);
        token = strtok(NULL, " ");
        i++;
    }
    free(user_input);
    return 0;
}