#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

void listFiles(const char *root, int range) {
	DIR *dir;
    	struct dirent *d;
    	struct stat sbuf;

	int i;
	dir = opendir(root);
	if (!dir)
	{
		perror("");
        	return;
	}
    	while ((d = readdir(dir)) != NULL) 
	{
	       	char buffer[1024];
        	snprintf(buffer, sizeof(buffer), "%s/%s\n", root, d->d_name);
        	lstat(buffer, &sbuf);
        	if (d->d_type == DT_DIR && d->d_name[0] != '.') {
        		if (strcmp(".", d->d_name) == 0 || strcmp("..", d->d_name) == 0)
                		continue;
        		for (i = 0; i < range; i++)
                		printf("|   ");
        		printf("|-- %s\n", d->d_name);
        		listFiles(buffer, range + 1);
       		}	
	 	else 
		{
			if(d->d_name[0] != '.')	
			{
            			for (i = 0; i < range; i++)
               				printf("|   ");
            			printf("|-- %s\n", d->d_name);
        		}	
		}
    	}			
    	if(closedir(dir) == -1)
	{

		perror("");
		return;

	}
}

int main(int argc, char *argv[])
{
	char *dir = ".";
	if (argc == 2)
		dir = argv[1];
	printf("%s\n", dir);
	listFiles(dir,0);
	printf("\n");	
	return 0;
}  

