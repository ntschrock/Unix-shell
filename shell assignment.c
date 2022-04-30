#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#define MAX_BUFFER 1024
#define MAX_ARGS 64
#define SEPARATORS " \t\n"

int main(int argc, char **argv)
{
    char buf[MAX_BUFFER];
    char *args[MAX_ARGS];
    char **arg;
    char *prompt = "#";
    char *temp[64];
    int ctr = 0;
    int process_grp = 0;

    //continues to read input until byebye
    while (!feof(stdin))
    {
        fputs(prompt, stdout);

        if (fgets(buf, MAX_BUFFER, stdin))
        {
            temp[ctr] = (char *)malloc(strlen(buf) + 1);
            strcpy(temp[ctr], buf);

            for (int i = 0; i < 64; i++)
            {
                args[i] = NULL;
            }

            arg = args;

            *arg++ = strtok(buf, SEPARATORS);

            while ((*arg++ = strtok(NULL, SEPARATORS)));
            if (args[0])
            { //begin the commands
                // movetodir command or chdir command-----------------------------------------------------------------------
                if (!strcmp(args[0], "movetodir"))
                {
                    //current directory
                    char currentdir[MAX_BUFFER];
                    //new directory
                    char newdir[MAX_BUFFER + 1];

                    if (chdir(args[1]) == 0)
                    {
                        chdir(args[1]);
                        strncpy(newdir, currentdir, sizeof(newdir)); // copy currentdir to newdir
                        printf("Changed to directory: %s\n", getcwd(currentdir, sizeof(currentdir)));
                    }
                    // error
                    else
                    {
                        printf("Error, no directory with name: %s\n", getcwd(currentdir, sizeof(currentdir)));
                    }
                }

                //whereami command or current directory command--------------------------------------------------------------
                if (!strcmp(args[0], "whereami"))
                {
                    // current directory
                    char currentdir[MAX_BUFFER];

                    if (getcwd(currentdir, sizeof(currentdir)) != NULL)
                    {
                        printf("Current dirrectory: %s\n", currentdir);
                    }
                    else
                    { //error
                        printf("Error finding where you are.\n");
                    }
                }

                //history command---------------------------------------------------------------------------------------------
                if (!strcmp(args[0], "history"))
                {
                    //command print
                    if (args[1] == NULL)
                    {
                        for (int i = 0; i <= ctr; i++)
                        {
                            printf("%d: %s.\n", i, temp[i]);
                        }
                    }
                    else
                    {
                        // clear history command
                        if (!strcmp(args[1], "[-c]"))
                        {
                            printf("Recent commands cleared.\n");
                            free(*temp);
                            char *temp[64];
                            temp[0] = (char *)(malloc(strlen(buf) + 1));
                            strcpy(temp[0], buf);
                            ctr = 0;
                        }
                    }
                }

                //byebye command or quit command---------------------------------------------------------------------------------
                if (!strcmp(args[0], "byebye"))
                {
                    break; //exits the loop
                }

                //replay number command------------------------------------------------------------------------------------------

                //start program command or run command---------------------------------------------------------------------------
                if (!strcmp(args[0], "run"))
                {
                    int argsSize = 0;
                    for (int a = 0; a < MAX_ARGS; a++)
                    {
                        if (args[a] == NULL)
                        {
                            break;
                        }
                        else
                        {
                            argsSize++;
                        }
                    }
                    if (args[1][0] == '/')
                    {
                        int rc = fork();
                        if (rc < 0)
                        { //error
                            fprintf(stderr, "failed to fork\n");
                            exit(1);
                        }
                        else if (rc == 0)
                        {
                            char *allargs[argsSize];
                            for (int b = 1; b < MAX_ARGS; b++)
                            {
                                if (args[b] == NULL)
                                {
                                    allargs[b - 1] = NULL;
                                    break;
                                }
                                else
                                {
                                    allargs[b - 1] = args[b];
                                }
                            }

                            chdir(allargs[0]);
                            execv(allargs[0], allargs);
                            printf("Couldn't execute %s.\n", allargs[0]); //error
                            exit(1);
                        }
                        else
                        {
                            waitpid(rc, NULL, 0);
                        }
                    }
                    else
                    {
                        int rc = fork();
                        if (rc < 0)
                        { //error
                            fprintf(stderr, "failed to fork\n");
                            exit(1);
                        }
                        else if (rc == 0)
                        {

                            char *allargs[argsSize];
                            for (int b = 1; b < MAX_ARGS; b++)
                            {
                                if (args[b] == NULL)
                                {
                                    allargs[b - 1] = NULL;
                                    break;
                                }

                                else
                                {
                                    allargs[b - 1] = args[b];
                                }
                            }

                            execv(allargs[0], allargs);
                            printf("Couldn't execute %s.\n", allargs[0]); //error
                            exit(1);
                        }
                        else
                        {
                            waitpid(rc, NULL, 0);
                        }
                    }
                }

                //background program command-------------------------------------------------------------------------------------
                if (!strcmp(args[0], "background"))
                {
                    int argsSize = 0;
                    for (int a = 0; a < MAX_ARGS; a++)
                    {
                        if (args[a] == NULL)
                        {
                            break;
                        }
                        else
                        {
                            argsSize++;
                        }
                    }
                    if (args[1][0] == '/')
                    {
                        int rc = fork();
                        if (rc < 0)
                        { //error
                            fprintf(stderr, "failed to fork\n");
                            exit(1);
                        }
                        else if (rc == 0)
                        {

                            printf("background process PID: %d)\n", (int)getpid());

                            char *allargs[argsSize];
                            for (int b = 1; b < MAX_ARGS; b++)
                            {
                                if (args[b] == NULL)
                                {
                                    allargs[b - 1] = NULL;
                                    break;
                                }
                                else
                                {
                                    allargs[b - 1] = args[b];
                                }
                            }
                            chdir(allargs[0]);
                            execv(allargs[0], allargs);
                            printf("Couldn't execute %s.\n", allargs[0]); //error
                            exit(1);
                        }
                        else
                        {
                            waitpid(-1, NULL, WNOHANG | WUNTRACED);
                        }
                    }
                    else
                    {
                        int rc = fork();
                        if (rc < 0)
                        { //error
                            fprintf(stderr, "failed to fork\n");
                            exit(1);
                        }
                        else if (rc == 0)
                        {
                            printf("background process PID: %d)\n", (int)getpid());

                            char *allargs[argsSize];
                            for (int b = 1; b < MAX_ARGS; b++)
                            {
                                if (args[b] == NULL)
                                {
                                    allargs[b - 1] = NULL;
                                    break;
                                }
                                else
                                {
                                    allargs[b - 1] = args[b];
                                }
                            }

                            execv(allargs[0], allargs);
                            printf("Couldn't execute %s.\n", allargs[0]); //error
                            exit(1);
                        }
                        else
                        {
                            waitpid(-1, NULL, WNOHANG | WUNTRACED);
                        }
                    }
                }

                //dalek PID command----------------------------------------------------------------------------------------------
                if (!strcmp(args[0], "dalek"))
                {
                    int pid = atoi(args[1]); //string to int
                    int killed = kill(pid, SIGKILL);
                    if (killed == 0)
                    {
                        printf("Program with PID: %d terminated\n", pid);
                    }
                    else
                    { //error
                        printf("Program with PID: %d was not able to be terminated\n", pid);
                    }
                }

                //dalekall command------------------------------------------------------------------------------------------------
                if (!strcmp(args[0], "dalekall"))
                {
                    int killed = killpg(process_grp, SIGKILL);
                    if (killed == 0)
                    {
                        printf("all programs successfully terminated\n");
                    }
                    else
                    { // killed == -1
                        printf("all programs unsuccessfully terminated\n");
                    }
                }

                ctr++;
            }
        }
    }
    // clear mem
    free(*temp);
    return 0;
}