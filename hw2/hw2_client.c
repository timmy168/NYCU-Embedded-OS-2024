//standard library
#include <stdio.h>   
#include <stdlib.h>  
#include <string.h>
#include <unistd.h>  
#include <signal.h>
#include <fcntl.h>

//socket
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFSIZE 256 //msg buffer size
#define errexit(format,arg...) exit(printf(format,##arg)) //error macro

//**********socket function**********//
int passivesock(const char *service, const char *transport, int qlen)
{
    struct servent *pse;  // pointer to service infomation entry
    struct sockaddr_in sin; // an Internet endpoint address
    int s, type;  

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;

    // Map service name to port number
    if ((pse = getservbyname(service, transport)))
        sin.sin_port = htons(ntohs((unsigned short)pse->s_port));
    else if ((sin.sin_port = htons((unsigned short)atoi(service))) == 0)
        errexit("can't find\"%s\"service entry\n", service);

    // Allocate a socket
    s = socket(PF_INET, SOCK_STREAM, 0);
    if (s < 0)
        errexit("can't create socket: %s\n", strerror(errno));

    // Force using socket address already in use 
    int yes = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    // Bind the socket
    if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
        errexit("can't bind to port %s: %s\n", service, strerror(errno));
    
    // Set the maximum number of waiting connection
    if (type == SOCK_STREAM && listen(s, qlen) < 0)
        errexit("can't listen on port %s: %s\n", service, strerror(errno));

    listen(s, qlen);

    return s;
}

int connectsock(const char *host, const char *service, const char *transport)
{
    struct hostent *phe; // pointer to host information entry 
    struct servent *pse; // pointer to service information entry 
    struct sockaddr_in sin; // an Internet endpoint address 
    int s , type;  

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;

    // Map service name to port number
    if ((pse = getservbyname(service, transport)))
        sin.sin_port = pse->s_port;
    else if ((sin.sin_port = htons((unsigned short)atoi(service))) == 0)
        errexit("can't get\"%s\"service entry\n", service);

    // Map host name to IP address, allowing for dotted decimal
    if ((phe = gethostbyname(host)))
        memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
    else if ((sin.sin_addr.s_addr = inet_addr(host))==INADDR_NONE)
        errexit("can't get\"%s\"host entry\n", host);

    // Allocate a socket
    s = socket(PF_INET, SOCK_STREAM, 0);
    if (s < 0)
        errexit("can't create socket: %s\n", strerror(errno));

    // Connect the socket
    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
        errexit("Can't connect to %s.%s: %s\n", host, service, strerror(errno));

    return s;
}

int main(int argc , char *argv[])
{

    // create socket
    int connfd = 0;  
    int n, read_fd;
    char input[BUFSIZE];
    char receiveMessage[BUFSIZE];
    int distance;

    if (argc != 3) errexit("Usage: %s <ip> <port>\n", argv[0]);

    // connect socket
    connfd = connectsock(argv[1] , argv[2] , "tcp");

    void CtrlCHandler(int signum) 
    {
        close(connfd);   
        exit(0);
    }

    do{  
        signal(SIGINT, CtrlCHandler); 
        printf("Client input: ");
         
        fgets(input, sizeof(input), stdin);  
        input[strcspn(input, "\n")] = 0;  
        
        //Send a message to server
        if ((n = write(connfd, input, strlen(input))) == -1)
            errexit("Error occurs when executing write()\n");
        memset(input, 0, BUFSIZE);    
        memset(receiveMessage, 0, BUFSIZE);
        if ((read_fd = read(connfd, receiveMessage, BUFSIZE)) == -1)
            errexit("Error occurs when executing read()\n");
        printf("receiveMessage is :\n%s\n", receiveMessage);

    }while(strcmp(receiveMessage, "Please wait a few minutes...\n") != 0);

    //Delivery has arrived and you need to pay %d$", total_money
    memset(receiveMessage, 0, BUFSIZE);
    if ((read_fd = read(connfd, receiveMessage, BUFSIZE)) == -1)  
        errexit("Error occurs when executing read()\n");
    printf("receiveMessage is :\n%s\n", receiveMessage);

    close(connfd);
    
    return 0;
}