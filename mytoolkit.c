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

#define MAX_INPUT_LENGTH 80
#define MAX_ARGS 10
#define MAXIMUM_HOURS 24

void print_prompt() {
    printf("$ ");
}


void execute_external(char** args) {
    pid_t pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
                perror("");
        }
    } else if (pid < 0) {
        printf("Error: Failed to fork child process.\n");
    } else {
        int status;
        waitpid(pid, &status, 0);

    }
}

void process_input_output_redirection(char** args, int n) {
    char *input_file = NULL;
    char *output_file = NULL;
    int input_fd, output_fd;
    int i, num_args = 0;
    int redirect_input = 0, redirect_output = 0;
    int input_stream = dup(STDIN_FILENO);
    int output_stream = dup(STDOUT_FILENO);
    int status, bidirectional = 0;

    char** new_args = (char**) malloc((n + 1) * sizeof(char*));

    for (i = 0; i < n; i++) {
        if (strcmp(args[i], "<") == 0) {
            if (i + 1 < n) {
                input_file = args[i + 1];
                redirect_input = 1;
                bidirectional++;
            }
            i++;
        } else if (strcmp(args[i], ">") == 0) {
            if (i + 1 < n) {
                output_file = args[i + 1];
                redirect_output = 1;
                bidirectional++;
            }
            i++;
        } else {
            new_args[num_args++] = args[i];
        }
    }

    new_args[num_args] = NULL;

    if (redirect_input == 1 && bidirectional != 2) {
        input_fd = open(input_file, O_RDONLY);
        int child = fork();
        if (input_fd == -1) {
            return;
        }
        if(child == 0){
                close(input_stream);
                dup2(input_fd, STDIN_FILENO);
                close(input_fd);
                execvp(new_args[0], new_args);
        }
        waitpid(child, &status, 0);
        dup2(STDIN_FILENO, input_stream);
    }

    else if (redirect_output == 1 && bidirectional != 2) {
        output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0744);
        int child = fork();
         if (output_fd == -1) {
            printf("Error: Failed to open output file.\n");
        }
        if(child == 0){
        close(output_stream);
        dup2(output_fd, STDOUT_FILENO);
        close(output_fd);
        execvp(new_args[0], new_args);
        }
        waitpid(child,&status,0);
        dup2(STDOUT_FILENO, output_stream);
    }
   else if(redirect_output == 1 && redirect_input == 1 && bidirectional == 2)
    {
        input_fd = open(input_file, O_RDONLY);
        output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0744);
        int child = fork();
        if(child == 0) {
                close(input_stream);
                close(output_stream);
                dup2(input_fd, STDIN_FILENO);
                dup2(output_fd, STDOUT_FILENO);
                close(input_fd);
                close(output_fd);
                execvp(new_args[0], new_args);
                waitpid(child,&status,0);
                dup2(STDOUT_FILENO, output_stream);
                dup2(STDIN_FILENO, input_stream);
        }
    }
}

void IORedirection(char **args, int n)
{
        process_input_output_redirection(args, n);
}


char** argParse(char* line, int* num_args) {
    char** args = (char**) malloc(MAX_ARGS * sizeof(char*));

    *num_args = 0;

    char* arg = strtok(line, " ");
    while (arg != NULL) {
        args[*num_args] = (char*) malloc((strlen(arg) + 1) * sizeof(char));
        strcpy(args[*num_args], arg);

        (*num_args)++;

        if (*num_args >= MAX_ARGS) {
            break;
        }

        arg = strtok(NULL, " ");
    }
    args[*num_args] = NULL;
    return args;
}


void mypwd()
{

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s\n", cwd);
	}
    else
    {
	    printf("Error: Failed to get current working directory.\n");
    }
}

int input_check(char **args,int num_args)
{
	int count = 0;
	int i;
 	for(i=0; i< num_args; i++)
	{
    	if((strcmp(args[i],"<")==0 || (strcmp(args[i],">")==0)))
    	{
            count = 1;
            return count;
    	}
    	else if (strcmp(args[i],"|")==0 )
    	{
        	count = 2;
        	return count;
        }
	}
}

int pipeHandler(char** args, int num_args) {
    int pid;
    int status;

    int i;
    int argument_count = 1;
    for (i = 1; i < num_args; i++) {
        if (strcmp(args[i], "|") == 0) {
            argument_count++;
        }
    }

    char* arguments[argument_count][1024];
    int argLen[argument_count];
    int argCount = 0;

    int arg_index = 0;
    for (i = 0; i < num_args; i++) {
        if (strcmp(args[i], "|") == 0) {
            arguments[argCount][arg_index] = NULL;
            argLen[argCount] = arg_index;
            argCount++;
            arg_index = 0;
        } else {
            arguments[argCount][arg_index] = args[i];
            arg_index++;
        }
    }
    arguments[argCount][arg_index] = NULL;
    argLen[argCount] = arg_index;

    int input_fd = 0;
    int pipefd[2];

    for (i = 0; i < argument_count; i++) {
        if (pipe(pipefd) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {

            if (i != 0) {
                if (dup2(input_fd, STDIN_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            if (i != argument_count - 1) {
                if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            close(pipefd[0]);
            close(pipefd[1]);

            execute_external(arguments[i]);
            exit(EXIT_SUCCESS);
        } else {

            if (input_fd != 0) {
                close(input_fd);
            }

            close(pipefd[1]);
            input_fd = pipefd[0];
        }
    }

    for (i = 0; i < argument_count; i++) {
        waitpid(pid, &status, 0);
    }
return 0;
}


void main() {
    char input[MAX_INPUT_LENGTH];
    int num_args;
    int i=0;
    char** args;
    char cwd[1024];
    while (1) {
        print_prompt();

        if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL) {
            printf("\n");
            exit(0);
        }

        input[strcspn(input, "\n")] = '\0';

        args = argParse(input, &num_args);

        if (num_args == 0) {

        } else if (strcmp(args[0], "myexit") == 0) {

            exit(0);
        }
        else if (strcmp(args[0], "mypwd") == 0) {

           mypwd();
        }

        else if (strcmp(args[0], "mycd") == 0)
        {
                if (num_args == 1)
                {
                        if (chdir(getenv("HOME")) != 0)
                        {
                                perror("mycd error");
                        }
                }
                 else if (num_args == 2)
                {
                        char* dir = args[1];
                        if (strncmp(dir, "~/", 2) == 0)
                        {
                                char* home_dir = getenv("HOME");
                                char* new_dir = malloc(strlen(home_dir) + strlen(dir) - 1);
                                sprintf(new_dir, "%s%s", home_dir, dir + 1);
                                if (chdir(new_dir) != 0)
                                {
                                        perror("mycd error");
                                }

                                free(new_dir);
                                getcwd(cwd,sizeof(cwd));
                        }
                        else
                        {
                                if (chdir(dir) != 0)
                                {
                                        perror("mycd error");
                                }
                        }
                }

                else
                {
                        printf("Usage: mycd [dir]\n");
                }
        }

        else if (num_args > 1)
        {

            int check = 0;
            check= input_check(args, num_args);

            if( check == 1)
                        {
                IORedirection(args, num_args);

            }
            else if(check == 2)
            {
                pipeHandler(args,num_args);
            }
            else 
            {
                execute_external(args);
            }
        }

        else
        {
                execute_external(args);
        }



        for (i = 0; i < num_args; i++)
        {
            free(args[i]);
        }
        free(args);
    }

}

