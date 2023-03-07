#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <libgen.h>
#include <errno.h>
#include <sys/resource.h>
#include <fcntl.h>

void timeout_handler(int sig) {

}


void mytimeout( float seconds, char* cmd_args[])
{
    pid_t pid = fork();
    if (pid == 0)
    {
        signal(SIGINT, timeout_handler);
        execvp(cmd_args[0], cmd_args);
        perror("execvp");

        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {

        perror("fork");
        exit(EXIT_FAILURE);
    }
    else
    {

        int status;
        pid_t result = waitpid(pid, &status, WNOHANG);
        if (result == -1)
        {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
        else if (result == 0)
        {
            sleep(1);
            kill(pid, SIGTERM);
        }
    }
}


void main(int argc, char *argv[])
{
int i;
 if (argc < 3)
                {
                        fprintf(stderr, "Usage: %s seconds cmd [args]\n", argv[0]);
                        exit(EXIT_FAILURE);
                }

                float seconds = atof(argv[1]);
                char* cmd_args[argc-1];
                for (i = 2; i < argc; i++)
                {
                        cmd_args[i-2] = argv[i];
                }
                cmd_args[argc-2] = NULL;

                mytimeout(seconds, cmd_args);

}
