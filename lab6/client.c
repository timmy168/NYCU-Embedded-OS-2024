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

    // Use protocol to choose a socket type
    if(strcmp(transport, "udp") == 0)
        type = SOCK_DGRAM;
    else  
        type = SOCK_STREAM;  

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

    // Use protocol to choose a socket type
    if(strcmp(transport, "udp") == 0)
        type = SOCK_DGRAM;
    else  
        type = SOCK_STREAM;

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
    //socket descriptor
    int client_fd, n; 

    struct sockaddr_in client_addr; 
    char msg[BUFSIZE];

    for(int i=0;i<argc;i++)
    {
        printf("%s\n",argv[i]);
    }

    //create socket and bind socket to port
    client_fd = connectsock(argv[1], argv[2], "tcp");

    //sending message
    memset(msg, 0, BUFSIZE);
    sprintf(msg, "%s %s %s", argv[3], argv[4], argv[5]);
    
    printf("Message:%s\n", msg);

    if((n = write(client_fd, msg, strlen(msg))) == -1)
    {
        errexit("Error occurs when executing write() %d\n", client_fd);
    }

    close(client_fd);
    return 0;
}