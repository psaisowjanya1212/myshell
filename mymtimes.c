#include <stdio.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#define MAXIMUM_HOURS 24

int errno;

void listFiles(char *path, int *hr_files)
{
    char p[1024];
    struct dirent *d;
    struct stat sbuf;
    DIR *dir=opendir(path);
    int i;

    if(dir==NULL)
    {
	if(EACCES == errno)
	{
		printf("This should not happen\n");
	}
	else if(ENOTDIR == errno)
	{
		printf ("%s is not a directory\n",path);
	}
	else if(ENOENT == errno)
	{
		printf("Files or directory doesnot exists\n");
	}
	
	exit(1);
    }                
    while ((d = readdir(dir)) != NULL)
    {
        if (strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0)
        {
            time_t t_now;
            time(&t_now);
            int hrs;
            sprintf(p, "%s/%s", path, d->d_name);
	    stat(p,&sbuf);
            if (S_ISREG(sbuf.st_mode))
            {
                time_t t_file = sbuf.st_mtime;
                double diffTime = difftime(t_now, t_file);
                if (diffTime <= 86400)
                {
                    hrs = (int)(diffTime / 3600);
                    hr_files[hrs]++;
                }
            }
            else if(S_ISDIR(sbuf.st_mode)) 
            {
                listFiles(p, hr_files);
            }

     }   }

    closedir(dir);
}

int main(int argc, char *argv[])
{
    char *directory;
    int hr_files[MAXIMUM_HOURS] = {0};
    int i;

    if (argc == 2)
    {
        directory = argv[1];
    }
    else
    {
        directory = ".";
    }

    listFiles(directory, hr_files);

    for (i = MAXIMUM_HOURS; i > 0; i--)
    {
        if (hr_files[i] >= 0)
        {
            time_t t_now;
            struct tm *timeInfo;
            char buf[100];
            time(&t_now);
            t_now = t_now - i * 3600;
	    timeInfo = localtime(&t_now);
            strftime(buf, sizeof(buf), "%a %b %d %T %Y", timeInfo);
            printf("%s: %d\n", buf, hr_files[i-1]);
        }
    }
    return 0;
}

