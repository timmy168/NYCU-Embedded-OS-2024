#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) 
{
    int fd;
    int len = strlen(argv[1]);
    char* name = (char*)malloc((len+1)* sizeof(char));
    strcpy(name, argv[1]);
    fd = open("/dev/mydev", O_WRONLY);

    if (fd < 0) {
        printf("Error opening device file!\n");
        return 1;
    }
    
    char published_letter;
    for(int i = 0; i < len; i++)
    {
        published_letter = name[i];
        printf("From writer.c: %c\n", published_letter);
        write(fd, &published_letter, 1);
        sleep(1);
    }

    close(fd);

    return 0;
}
