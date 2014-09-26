//  Jeff Tseng
//  CS 307
//  Programming project 2

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <termios.h>

void print_prompt(); //prints current directory

int main( )
{
    int status;
    int i = 0;
    int k = 0;
    int s = 0;
    size_t top = 0;
    int cursor = 0;
    int execvp_ok;
    int chdir_ok;
    bool is_exit = false;
    bool blrud = false;
    bool history_full = false;
    bool up_pressed = false;
    bool last_up = false;
    bool last_down = false;
    size_t bytes_read;
    char **command;
    char *token;
    char *buf = (char*)calloc(20, sizeof(char));
    char *user_input = (char*)calloc(256, sizeof(char));
    char *history[11];
    
    setbuf(stdout, NULL);  //removes buffering for stdout;
    
    struct termios origConfig;
    tcgetattr(0, &origConfig);
    struct termios newConfig = origConfig;
    newConfig.c_lflag &=~(ICANON|ECHO);
    newConfig.c_cc[VMIN] = 10;
    newConfig.c_cc [VTIME] = 1;
    tcsetattr(0, TCSANOW, &newConfig);
    
    while (is_exit != true)
    {
        if (i == 0 && blrud == false)
        {
            print_prompt();
        }
        bytes_read = read(0, &buf[0], 20);
        if (buf[0] == 27)
            //*********    UP ARROW  *************
            if(buf[1] == 91)
            {
                if (buf[2] == 65)
                {
                    if (last_down == true)
                    {
                        if (cursor != top)
                            cursor--;
                        cursor--;
                    }
                    last_down = false;
                    if (top == 0 && history_full == false)
                    {
                        buf = (char*)calloc(20, sizeof(char));
                        blrud = true;
                        continue;
                    }
                    else
                    {
                        up_pressed = true;
                        last_up = true;
                        for (s = 0; s < i;s++)
                            printf("\b \b");
                        printf("%s", history[cursor]);
                        i = strlen(history[cursor]);
                        strcpy(user_input, history[cursor]);
                        if (history_full == true)
                        {
                            if (cursor != top)
                            {
                                if (cursor == 0)
                                    cursor = 10;
                                cursor--;
                            }
                        }
                        
                        else if (cursor > 0)
                        {
                            cursor--;
                        }
                        buf = (char*)calloc(20, sizeof(char));
                        continue;
                    }
                }
                //*********************     DOWN ARROW    ********************
                
                if (buf[2] == 66)
                {
                    blrud = true;
                    if (up_pressed == true)
                    {
                        {
                            if (last_up == true)
                            {
                                if (cursor != 0 && cursor != top)
                                    cursor++;
                                cursor++;
                            }
                            last_up = false;
                            for (s = 0; s < i;s++)
                                printf("\b \b");
                            i = 0;
                            if (cursor != top)
                            {
                                printf("%s", history[cursor]);
                                i = strlen(history[cursor]);
                                strcpy(user_input, history[cursor]);
                            }
                            if (cursor != top)
                            {
                                if (cursor == 9)
                                    cursor = -1;
                                cursor++;
                                last_down = true;
                            }
                            buf = (char*)calloc(20, sizeof(char));
                            continue;
                        }
                    }
                    
                    buf = (char*)calloc(20, sizeof(char));
                    continue;
                    
                }
            }
        //********************* USER ENTERS \n ***************
        if (strcmp(buf, "\n") == 0)
        {
            up_pressed = false;
            last_down = false;
            last_up = false;
            printf("\n");
            if (strcmp(user_input, "") == 0)
            {
                blrud = false;
                free (buf);
                buf = (char*)calloc(20, sizeof(char));
                continue;
            }
            
            history[top] = (char*)calloc(strlen(user_input), sizeof(char));
            strcpy(history[top], user_input);
            top++;
            cursor = top-1;
            if (top >= 10)
            {
                top = 0;
                history_full = true;
            }
            command = (char**)calloc(strlen(user_input)*2, sizeof(char));
            token = strtok(user_input, " ");
            
            for(k = 0; token != NULL; k++)
            {
                command[k] = (char*)calloc(strlen(token), sizeof(char));
                strcpy(command[k], token);
                token = strtok(NULL, " ");
            }
            
            if (strcmp(command[0], "cd") == 0)
            {
                chdir_ok = chdir((user_input+3));
                
                if (chdir_ok < 0)
                {
                    perror(NULL);
                }
                i = 0;
                free(command);
                free(user_input);
                blrud = false;
                user_input = (char*)calloc(256, sizeof(char));
                continue;
            }
            
            else if (strcmp(command[0], "exit") == 0)
            {
                is_exit = true;
                break;
            }
            
            else
            {
                unsigned int pid = fork();
                
                if (pid == 0)
                {
                    execvp_ok = execvp(command[0], command);
                    
                    if (execvp_ok < 0)
                    {
                        perror(NULL);
                    }
                    exit(0);
                }
                else
                {
                    waitpid(pid, &status, WUNTRACED);
                }
            }
            i = 0;
            free(command);
            free(user_input);
            blrud = false;
            user_input = (char*)calloc(256, sizeof(char));
            free (buf);
            buf = (char*)calloc(20, sizeof(char));
            continue;
        }
        
        user_input[i] = buf[0];
        printf("%c", user_input[i]);
        blrud = false;
        i++;
    }
    
    tcsetattr(0, TCSANOW, &origConfig);
    return 0;
}

void print_prompt( )
{
    char *curr_dir = (char*)malloc(100);
    
    getcwd(curr_dir, 100);
    printf("%s> ", curr_dir);
    free(curr_dir);
}