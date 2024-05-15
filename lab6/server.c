//standard library
#include <stdio.h>   
#include <stdlib.h>  
#include <string.h>
#include <unistd.h>  
#include <signal.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <pthread.h>

//socket
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

# define BUFSIZE 256 //msg buffer size
#define errexit(format,arg...) exit(printf(format,##arg)) //error macro

//check function result of semget, semctl, pthread_create
#define checkResults(string, val){ \
    if(val) \
    { \
        printf("Failed with %d when executing %s", val, string); \
        exit(1); \
    } \
}

int server_fd, client_fd; //socket descriptor

//********************Semaphore********************//
int s; //semaphore
#define DOODLE_SEM_KEY 1010
#define SEM_MODE 0666
#define NUMTHREADS 5

// P () - returns 0 if OK; -1 if there was a problem 
int P(int s)
{
    struct sembuf sop;  // the operation parameters 
    sop.sem_num = 0;    // access the 1st (and only) sem in the array 
    sop.sem_op = -1;    // wait...
    sop.sem_flg = 0;    // no special options needed 
    
    if (semop (s, &sop, 1) < 0) 
    {
        fprintf(stderr,"P(): semop failed: %s\n",strerror(errno));
        return -1;
    } 
    else 
    {
        return 0;
    }
}

// V() - returns 0 if OK; -1 if there was a problem 
int V(int s)
{
    struct sembuf sop;  // the operation parameters 
    sop.sem_num = 0;    // the 1st (and only) sem in the array 
    sop.sem_op = 1;     // signal 
    sop.sem_flg = 0;    // no special options needed 

    if (semop(s, &sop, 1) < 0) 
    {
        fprintf(stderr,"V(): semop failed: %s\n",strerror(errno));
        return -1;
    } 
    else 
    {
        return 0;
    }
}
//********************end of Semaphore********************//

//********************Signal Handler*******************//
void Ctrl_C_Handler(int signum) 
{
    close(server_fd);
    // remove semaphore
    if(semctl (s,0,IPC_RMID, 0) < 0)
    {
        fprintf(stderr, "Server: Failed to find semaphore: %d.\n", DOODLE_SEM_KEY);
        exit(1);
    }
    printf("Semaphore %d has been remove successfully.\n", DOODLE_SEM_KEY);
    exit(0);
}

//*******************socket function*******************//
void publish_message(int client_fd, char* massage)
{
    char msg[BUFSIZE];
    memset(msg, 0, BUFSIZE);
    strcpy(msg, massage);   
    // strcat(msg, "\n");
    printf("Sever sending to client: %s\n", msg);  

    int n;
    if ((n = write(client_fd, msg, BUFSIZE)) == -1)
    { 
        errexit("Error occurs when executing write()\n");
    }
}

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
//********************End of socket function********************//

//********************Bank Parmeters and functions********************//
static unsigned int total_deposit_money = 0;
void *bankOperation(void *socket_fd)
{
    //argument
    int time, amount;
    char operation[BUFSIZE];

    //socke descriptor
    long client_fd;
    client_fd = (long)socket_fd;

    //recieved message and argument
    char received[BUFSIZE], arg[3][BUFSIZE];
    
    //devide the char
    memset(received, 0, sizeof(received));
    read(client_fd, received, BUFSIZE);
    close(client_fd); 

    char *token;
    token = strtok(received, " "); 
    for(int i = 0; i < 3; i++)
    {
        strcpy(arg[i], token);    
        token = strtok(NULL, " ");
    }
    strcpy(operation, arg[0]);
    amount = atoi(arg[1]);
    time = atoi(arg[2]);
    
    printf("Operation:%s, Times:%d, Amount:%d\n",operation, time, amount);
    
    
    //********************Critical Section********************//
    //This section will Access Shared Resources
    if(strcmp("deposit", operation) == 0)
    {
        for(int i = 0; i < time; i++)
        {
            P(s);
            total_deposit_money += amount;
            printf("After deposit: $%d\n", total_deposit_money);
            usleep(5);
            V(s);
        }     
    }
    else if(strcmp("withdraw", operation) == 0)
    {
        for(int i = 0; i < time; i++)
        {
            P(s);
            total_deposit_money -= amount;
            printf("After withdraw: $%d\n", total_deposit_money);
            usleep(5);
            V(s);
        }     
    }
    /**************** Critical Section *****************/
    
    return NULL;
}

// main function 
int main(int argc , char *argv[])
{   
    //socket
    signal(SIGINT, Ctrl_C_Handler);
    int check_result;
    long client_fd;
    struct sockaddr_in server_addr; 
    socklen_t addr_len = sizeof(server_addr);
    
    //thread
    pthread_t *thread;

    if (argc != 2) 
        errexit("Usage: %s <PORT>\n", argv[0]);
    
    //semaphore
    s = semget(DOODLE_SEM_KEY, 1, IPC_CREAT | SEM_MODE);
    if(s < 0)
    {
        fprintf(stderr, "Semaphore %d  creation failed: %s\n", DOODLE_SEM_KEY, strerror(errno));
        exit(1);
    }
    printf("Create Semaphore %d \n", DOODLE_SEM_KEY);

    if(semctl(s, 0, SETVAL, 1) < 0)
    {
        fprintf(stderr, "Unable to initialize semaphore: %s\n", strerror(errno));
        exit(0);
    }
    printf("Semaphore %d has been initialized to %d\n", DOODLE_SEM_KEY, 1);
    
    fflush(stdin);

    //create socket and bind socket to port
    server_fd = passivesock(argv[1], "tcp", 10);
    while(client_fd = accept(server_fd, (struct sockaddr *)&server_addr, (socklen_t *)&addr_len))
    {   
        thread = (pthread_t*)malloc(sizeof(pthread_t));

        //create thread
        check_result = pthread_create(thread, NULL, bankOperation, (void *)client_fd);
        checkResults("pthread_create()\n", check_result);

        //detach thread
        check_result = pthread_detach(*thread);
        checkResults("pthread_detach()\n", check_result);

        free(thread); 
    }

    return 0;
}