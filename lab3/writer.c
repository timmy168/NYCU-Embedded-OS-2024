#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv) {
    int fd;

    fd = open("/dev/etx_device", O_WRONLY);

    if (fd < 0) {
        printf("Error opening device file!\n");
        return 1;
    }
    
    // // Clear the contents of the file
    // if (truncate("/dev/etx_device", 0) < 0) {
    //     printf("Error clearing device file!\n");
    //     return 1;
    // }

    write(fd, argv[1], strlen(argv[1]));

    close(fd);

    return 0;
}