#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[])
{
    	if (argc < 2) {
        	printf("Error: Argument missing\nUsage: mytime.x cmd [arguments]\n");
        	return 1;
    	}	

   	pid_t pid;
	pid = fork();
        long seconds;
	long micro;
    	struct timeval start, end;
	/* start and end variables are used here to calculate the elapsed time of program*/
    	
	gettimeofday(&start, NULL);
	
	/*parent process is executing*/
	if( pid > 0)
	{
        	int status;
        	struct rusage usage;
        	wait4(pid, &status, 0, &usage);
		/*pid is the process id of child process to wait for, status will have the 
 		* exit status of child process and usage will have the child's resource usage.*/ 
        
		gettimeofday(&end, NULL);
		
        	seconds = (end.tv_sec - start.tv_sec);
        	micro = (end.tv_usec - start.tv_usec);

        	printf("\n%ld.%06ldu %ld.%06lds 0:0%6f\n ", usage.ru_utime.tv_sec, 
			usage.ru_utime.tv_usec, usage.ru_stime.tv_sec, usage.ru_stime.tv_usec, seconds+micro*1e-6);
	}    
		
	else if (pid == 0) 
	{
        	execvp(argv[1], &argv[1]);
        	printf("%s: Command not found.", argv[1]);
        	return 1;
    	}
	
	/*no child process created*/ 
	else 
	{
	        perror("fork");
        	return 1;
    	} 

    return 0;
}

