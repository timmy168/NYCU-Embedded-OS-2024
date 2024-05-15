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
# define BUFSIZE 256 //msg buffer size
#define errexit(format,arg...) exit(printf(format,##arg)) //error macro

//shops
enum shop{Dessert, Beverage, Diner}; 

//**********Ordering Functions**********//
// specify which shop does the meal belongs to
enum shop select_shop(char* meal) 
{
    if(strcmp(meal, "cookie") == 0 || strcmp(meal, "cake") == 0) return Dessert;
    else if(strcmp(meal, "tea") == 0 || strcmp(meal, "boba") == 0) return Beverage;
    else if(strcmp(meal, "fried-rice") == 0 || strcmp(meal, "Egg-drop-soup") == 0) return Diner;
}

// calculate the total price of the order in the corresponding shop
int dessert_order(char* meal, int count)
{
    if(strcmp(meal, "cookie") == 0) return 60 * count;
    else if(strcmp(meal, "cake")==0) return 80 * count;
    else return 0;
}

int beverage_order(char* meal, int count)
{
    if(strcmp(meal, "tea") == 0) return 40 * count;
    else if(strcmp(meal, "boba") == 0) return 70 * count;
    else return 0;
}

int diner_order(char* meal, int count)
{
    if(strcmp(meal, "fried-rice") == 0) return 120 * count;
    else if(strcmp(meal, "Egg-drop-soup") == 0) return 50 * count;
    else return 0;
}

//process the order to the correct format
char* process_dessert_order(char* ordered, char* meal, int count)
{
    char* msg = malloc(BUFSIZE);  
    char ordered_meal_1[15];
    int ordered_count_1, ordered_count_2;
    int multi_item = 0;
    
    if (strchr(ordered, '|') != NULL) multi_item = 1;

    if (multi_item)
    {  
        sscanf(ordered, "%*s %d|%*s %d", &ordered_count_1, &ordered_count_2);   
        if (strcmp("cookie", meal) == 0)
        {  
            count += ordered_count_1;
            sprintf(msg, "%s %d|%s %d\n", "cookie", count, "cake", ordered_count_2);  
        }
        else
        {                                  
            count += ordered_count_2;
            sprintf(msg, "%s %d|%s %d\n", "cookie", ordered_count_1, "cake", count); 
        }
    }
    else
    {  
        sscanf(ordered, "%s %d", ordered_meal_1, &ordered_count_1);
        if (strcmp(ordered_meal_1, meal) == 0)
        {  
            count += ordered_count_1;
            sprintf(msg, "%s %d\n", meal, count);  
        }
        else
        {                                  
            if (strcmp("cookie", meal) == 0)
            {   
                sprintf(msg, "%s %d|%s %d\n", "cookie", count, "cake", ordered_count_1);  
            }
            else
            {   
                sprintf(msg, "%s %d|%s %d\n", "cookie", ordered_count_1, "cake", count); 
            }
        }
    }
    return msg;
}


char* process_beverage_order(char* ordered, char* meal, int count)
{
    char* msg = malloc(BUFSIZE);  
    char ordered_meal_1[15];
    int ordered_count_1, ordered_count_2;
    int multi_item = 0;
    
    if (strchr(ordered, '|') != NULL) multi_item = 1;

    if (multi_item)
    {  
        sscanf(ordered, "%*s %d|%*s %d", &ordered_count_1, &ordered_count_2);   
        if (strcmp("tea", meal) == 0)
        {  
            count += ordered_count_1;
            sprintf(msg, "%s %d|%s %d\n", "tea", count, "boba", ordered_count_2);  
        }
        else
        {                                  
            count += ordered_count_2;
            sprintf(msg, "%s %d|%s %d\n", "tea", ordered_count_1, "boba", count); 
        }
    }
    else
    { 
        sscanf(ordered, "%s %d", ordered_meal_1, &ordered_count_1);
        if (strcmp(ordered_meal_1, meal) == 0)
        { 
            count += ordered_count_1;
            sprintf(msg, "%s %d\n", meal, count);  
        }
        else
        {                                
            if (strcmp("tea", meal) == 0)
            {   
                sprintf(msg, "%s %d|%s %d\n", "tea", count, "boba", ordered_count_1);  
            }else
            {                          
                sprintf(msg, "%s %d|%s %d\n", "tea", ordered_count_1, "boba", count); 
            }
        }
    }
    return msg;
}


char* process_diner_order(char* ordered, char* meal, int count)
{
    char* msg = malloc(BUFSIZE);  
    char ordered_meal_1[15];
    int ordered_count_1, ordered_count_2;
    int multi_item = 0;
    
    if (strchr(ordered, '|') != NULL) multi_item = 1;

    if (multi_item)
    {  
        sscanf(ordered, "%*s %d|%*s %d", &ordered_count_1, &ordered_count_2);   
        if (strcmp("fried-rice", meal) == 0)
        {  
            count += ordered_count_1;
            sprintf(msg, "%s %d|%s %d\n", "fried-rice", count, "Egg-drop-soup", ordered_count_2);  
        }
        else
        {                                  
            count += ordered_count_2;
            sprintf(msg, "%s %d|%s %d\n", "fried-rice", ordered_count_1, "Egg-drop-soup", count); 
        }
    }
    else{  
        sscanf(ordered, "%s %d", ordered_meal_1, &ordered_count_1);
        if (strcmp(ordered_meal_1, meal) == 0)
        {
            count += ordered_count_1;
            sprintf(msg, "%s %d\n", meal, count);  
        }
        else
        {                                  
            if (strcmp("fried-rice", meal) == 0)
            { 
                sprintf(msg, "%s %d|%s %d\n", "fried-rice", count, "Egg-drop-soup", ordered_count_1);  
            }
            else
            {                              
                sprintf(msg, "%s %d|%s %d\n", "fried-rice", ordered_count_1, "Egg-drop-soup", count); 
            }
        }
    }
    return msg;
}

//**********socket function**********//
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

//**********Signal Handler**********//
void Ctrl_C_Handler(int signum) 
{
    exit(0);
}

int main(int argc , char *argv[])
{   
    // socket
    int server_fd, client_fd; //socket descriptor
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);
    char received_message[BUFSIZE], msg[BUFSIZE];

    // order parameter
    char order[6], meal[15];
    int count, total_price = 0;
    enum shop order_shop; // 0:Dessert Shop, 1:Beverage Shop, 2:Diner
    int distance[3] = {3, 5, 8};
    char ordered[BUFSIZE]; 
    int first_order = 0;

    // read writer descriptor
    int read_fd, write_fd;
    
    if (argc != 2)
        errexit("Usage: %s <PORT>\n", argv[0]);

    //create socket and bind socket to port
    server_fd = passivesock(argv[1], "tcp", 10);

    while (1)
    {
        // waiting for connection, accept connection from clients 
        client_fd = accept(server_fd, (struct sockaddr *)&server_addr, &addr_len);
        if (client_fd < 0)
            errexit("Error occurs when executing accept()\n");        
        
        // Initial the variable when a new client is connected
        memset(ordered, 0, BUFSIZE);  
        first_order = 0;
        total_price = 0;
        while(1)
        {
            signal(SIGINT, Ctrl_C_Handler); 
            //received_message from client: 1.shop list, 2.order, 3.confirm, cancel 
            memset(received_message, 0, BUFSIZE);
            if ((read_fd = read(client_fd, received_message, BUFSIZE)) < 0)
                errexit("Error occurs when executing read()\n");

            //**********shop list**********//
            if (strcmp(received_message, "shop list") == 0)
            {
                publish_message(client_fd, "Dessert shop:3km\n- cookie:$60|cake:$80\nBeverage shop:5km\n- tea:$40|boba:$70\nDiner:8km\n- fried-rice:$120|Egg-drop-soup:$50\n");
            }
            //**********confirm**********//
            else if(strcmp(received_message, "confirm") == 0)
            {
                if (strcmp(ordered, "") == 0)
                {
                    publish_message(client_fd, "Please order some meals\n");
                }
                else
                {
                    publish_message(client_fd, "Please wait a few minutes...\n");
                    sleep(distance[order_shop]);
                    sprintf(msg, "Delivery has arrived and you need to pay %d$\n", total_price); 
                    publish_message(client_fd, msg);
                    break;
                }
            }
            //**********cancel**********//
            else if(strcmp(received_message, "cancel") == 0)
            {
                break;
            }
            //**********order**********//
            else
            {  
                //order
                sscanf(received_message, "%s %s %d", order, meal, &count);

                //Determine the meals are in the same shop or not
                if (first_order) // Isn't the first order
                {   
                    enum shop order_shop2 = select_shop(meal);
                    if (order_shop2 == order_shop) //new order shop = old order shop
                    {   
                        char *new_ordered;
                        switch (order_shop2) //calculate order's price
                        {
                            case Dessert:
                                total_price += dessert_order(meal, count);
                                new_ordered = process_dessert_order(ordered, meal, count);
                                break;
                            case Beverage:
                                total_price += beverage_order(meal, count);
                                new_ordered = process_beverage_order(ordered, meal, count);
                                break;
                            case Diner:
                                total_price += diner_order(meal, count);
                                new_ordered = process_diner_order(ordered, meal, count);
                                break;
                            default:
                                break;
                        }
                        strcpy(ordered, new_ordered); //determine the order is the same or not
                        free(new_ordered); //prevent interrupt
                    }
                    publish_message(client_fd, ordered);   
                }
                else //first order
                {          
                    first_order = 1;
                    order_shop = select_shop(meal); 
                    switch (order_shop)  //calculate order's price
                    {
                        case Dessert:
                            total_price += dessert_order(meal, count);
                            break;
                        case Beverage:
                            total_price += beverage_order(meal, count);
                            break;
                        case Diner:
                            total_price += diner_order(meal, count);
                            break;
                        default:
                            break;
                    }
                    sprintf(ordered, "%s %d\n", meal, count);
                    publish_message(client_fd, ordered); 
                }
            }
        }
        close(client_fd);
    }

    return 0;
}