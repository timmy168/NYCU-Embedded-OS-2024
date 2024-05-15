#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h> // waitpid()
#include <sys/socket.h>
#include <arpa/inet.h> // sockaddr related

pid_t childpid;
int server_fd;

// signal handler
void handler(int signum) 
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// child function
void childfunc(int new_socket)
{
    // const char *welcome = "Conected to the server success";
    // send(new_socket, welcome, strlen(welcome), 0);
    dup2(new_socket, STDOUT_FILENO);
    execl("/usr/games/sl", "/usr/games/sl", "-l", NULL);
    execlp("sl", "sl","-l", NULL);
    close(new_socket);
	exit(0);
}

int main(int argc, char const *argv[])
{
    
    struct sockaddr_in address; //socket address for IPV4
    int opt = 1, yes = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    signal(SIGCHLD, handler);

    // Create socket server
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) //IPV4, tcp
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Setting Socket Opt
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)))
    {
        perror("Set Socket Failed");
        exit(EXIT_FAILURE);
    }

    // Setting Socket Address
    address.sin_family = AF_INET;
    address.sin_port = htons((u_short)atoi(argv[1]));
    address.sin_addr.s_addr = INADDR_ANY;
    

    // Binding
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind Socket Failed");
        exit(EXIT_FAILURE);
    }

    // Listen Socket
    if (listen(server_fd, 3) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Waiting Client to Connect...\n");

    int new_socket;
    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen))) 
    {
        childpid = fork();
        if (childpid >= 0) // fork sucessed
        {
            if (childpid == 0) //child sucesses
            { 
                childfunc(new_socket);
            } 
            else //print child pid
            {
                printf("Train ID: %d\n",childpid);
            }
        } 
        else // fork return -1 : fork error 
        {
            perror("fork error"); 
            exit(0);
        }
    }
    close(server_fd);
    return 0;
}