//std library
#include <stdio.h>   
#include <stdlib.h>  
#include <string.h>
#include <unistd.h>  
#include <signal.h>
#include <fcntl.h>

// semaphore
#include <sys/sem.h>  

// shared memory
#include <sys/shm.h>  
#include <sys/ipc.h>    
#include <sys/types.h>  

// thread
#include <pthread.h>
#define NUMOFTHREADS 40

// socket
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#define checkResults(string, val) \
{ \
    if (val){ \
        printf("Failed with %d at %s\n", val, string); \
        exit(1); \
    } \
} \

#define errexit(format,arg...) exit(printf(format,##arg))
#define BUFSIZE 256

// delivery man
int delivery_man_1_income = 0;
int delivery_man_2_income = 0;
int delivery_man_1_customer = 0;
int delivery_man_2_customer = 0;
int delivery_man_1_delivery_time = 0;
int delivery_man_2_delivery_time = 0;

// global variable
enum shop{Dessert, Beverage, Diner}; 

// thread id
int total_thread_count=0;
__thread int current_thread_id = 0;

//mutex semaphore
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex4 = PTHREAD_MUTEX_INITIALIZER;

//**********Ordering Functions**********//
// specify which shop does the meal belongs to
int select_shop(char* meal)
{
    if(strcmp(meal, "cookie") == 0 || strcmp(meal, "cake") == 0)
        return Dessert;
    else if(strcmp(meal, "tea") == 0 || strcmp(meal, "boba") == 0)
        return Beverage;
    else if(strcmp(meal, "fried-rice") == 0 || strcmp(meal, "Egg-drop-soup") == 0)
        return Diner;
}

// calculate the total price of the order in the corresponding shop
int dessert_order(char* meal, int count)
{
    if(strcmp(meal, "cookie") == 0)
    {
        return 60 * count;
    }
    else if(strcmp(meal, "cake")==0)
    {
        return 80 * count;
    }
    else
        return 0;
}

int beverage_order(char* meal, int count)
{
    if(strcmp(meal, "tea") == 0)
    {
        return 40 * count;
    }
    else if(strcmp(meal, "boba") == 0)
    {
        return 70 * count;
    }
    else
        return 0;
}

int diner_order(char* meal, int count)
{
    if(strcmp(meal, "fried-rice") == 0)
    {
        return 120 * count;
    }
    else if(strcmp(meal, "Egg-drop-soup") == 0)
    {
        return 50* count;
    }
    else
        return 0;
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
void publish_message(int client_fd, char* message)
{
    char msg[BUFSIZE];
    memset(msg, 0, BUFSIZE);
    strcpy(msg, message);

    // If the last char isn't "\n", add it
    int len = strlen(msg);
    if (len > 0 && msg[len - 1] != '\n') {
        strcat(msg, "\n");
    }

    printf("Server sending to thread #%d: %s", current_thread_id, msg);  
    // printf("End of Server Message\n");

    int n;
    if ((n = write(client_fd, msg, strlen(msg))) == -1)
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

//recieve msg from the client
void message_callback(int client_fd, char* message) {
    memset(message, 0, BUFSIZE);

    fd_set read_fds;
    struct timeval timeout;
    int rv;

    FD_ZERO(&read_fds);
    FD_SET(client_fd, &read_fds);

    // Set timeout to 5 seconds (adjust as needed)
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    rv = select(client_fd + 1, &read_fds, NULL, NULL, &timeout);
    if (rv == -1) {
        errexit("Error occurs when executing select()\n");
    } else if (rv == 0) {
        printf("Timeout occurred! No data after 5 seconds.\n");
        strcpy(message, "No response from client\n"); // Handle timeout case as needed
    } else {
        int rn;
        if ((rn = read(client_fd, message, BUFSIZE)) == -1) {
            errexit("Error occurs when executing read()\n");
        }
        if (strlen(message) > 0) {
            printf("Message_Callback from thread #%d: %s", current_thread_id, message);
        }
    }
}

//**********socket function**********//

//**********Thread Function**********//
void *get_order(void* client_fd_ptr)
{  
    // socket
    int client_fd = *(int*)client_fd_ptr;
    current_thread_id = client_fd;
    printf("get_order in thread #%d\n", current_thread_id);
    char received_message[BUFSIZE], msg[BUFSIZE];

    // order parameter
    char order[6], meal[15];
    int count = 0, total_price = 0;
    enum shop order_shop, other_shop; // 0:Dessert Shop, 1:Beverage Shop, 2:Diner
    int shop_distance[3] = {3, 5, 8};
    char ordered[BUFSIZE]; 
    memset(ordered, 0, BUFSIZE); 
    int first_order = 0;  

    int temp = 0, temp2 = 0;

    while(1)
    {  
        //received_message from client: 1.shop list, 2.order, 3.confirm, cancel 
        message_callback(client_fd, received_message);
        
        //**********shop list**********//
        if (strcmp(received_message, "shop list\n") == 0)
        {
            publish_message(client_fd, "Dessert shop:3km\n- cookie:$60|cake:$80\nBeverage shop:5km\n- tea:$40|boba:$70\nDiner:8km\n- fried-rice:$120|Egg-drop-soup:$50\n");
        }
        //**********confirm**********//
        else if(strcmp(received_message, "confirm\n") == 0)
        {  
            if (strcmp(ordered, "") == 0)
            {
                publish_message(client_fd, "Please order some meals\n");
            }
            else
            {   
                printf("----------------------------\n");
                printf("#%d delivery_man_1 delivery time: %d\n", current_thread_id ,delivery_man_1_delivery_time);
                printf("#%d delivery_man_2 delivery time: %d\n", current_thread_id ,delivery_man_2_delivery_time);
                printf("----------------------------\n");
                // deleiver the costumer to the fewer time delivery man
                if (delivery_man_1_delivery_time <= delivery_man_2_delivery_time)
                {  
                    printf("-----------------------------------\n");
                    printf("#%d delivery_man_1 is fater!!\n", current_thread_id);
                    pthread_mutex_lock(&mutex1);
                    delivery_man_1_delivery_time += shop_distance[order_shop];  
                    pthread_mutex_unlock(&mutex1);

                    if (delivery_man_1_delivery_time > 30)
                    {
                        publish_message(client_fd,"Your delivery will take a long time, do you want to wait?\n");
                        // do{
                        printf("-----------------------------------\n");
                        printf("Costumer Deciding\n");
                        memset(received_message, 0, BUFSIZE);
                        message_callback(client_fd, received_message);
                        printf("Costumer Decided\n");
                        printf("-----------------------------------\n");
                        // }while((strcmp(received_message, "No") != 0) && (strcmp(received_message, "Yes") == 0));
                        if (strcmp(received_message, "No\n") == 0)
                        {
                            printf("Costumer said no\n");
                            pthread_mutex_lock(&mutex1);
                            delivery_man_1_delivery_time -= shop_distance[order_shop]; //protect????
                            pthread_mutex_unlock(&mutex1);
                            break;
                        }
                        else
                        {  
                            printf("#%d delivery_man_1 go!!\n", current_thread_id);
                            publish_message(client_fd, "Please wait a few minutes...");     
                            //*****creitical section*****//
                            pthread_mutex_lock(&mutex3);
                            sleep(shop_distance[order_shop]);
                            delivery_man_1_customer += 1;
                            delivery_man_1_income += total_price; 

                            pthread_mutex_lock(&mutex1);
                            delivery_man_1_delivery_time -= shop_distance[order_shop];
                            pthread_mutex_unlock(&mutex1);
                            pthread_mutex_unlock(&mutex3);
                            //*****creitical section*****//
                            sprintf(msg, "Delivery has arrived and you need to pay %d$\n", total_price);
                            publish_message(client_fd, msg);
                            break;
                        }
                    }
                    //**********deliver 1 delivering**********//
                    //increase that delivery man's total costumer, total income and sleep for delivering
                    printf("#%d delivery_man_1 go!!\n", current_thread_id);
                    publish_message(client_fd, "Please wait a few minutes...");  
        
                    //*****creitical section*****//
                    pthread_mutex_lock(&mutex3);
                    sleep(shop_distance[order_shop]);
                    delivery_man_1_customer += 1;
                    delivery_man_1_income += total_price; 
                    
                    pthread_mutex_lock(&mutex1);
                    delivery_man_1_delivery_time -= shop_distance[order_shop];
                    pthread_mutex_unlock(&mutex1);
                    pthread_mutex_unlock(&mutex3);
                    //*****creitical section*****//
                    
                    sprintf(msg, "Delivery has arrived and you need to pay %d$\n", total_price); 
                    publish_message(client_fd, msg);
                    break;
                }
                else
                {
                    printf("-----------------------------------\n");
                    printf("#%d delivery_man_2 is fater!!\n", current_thread_id);
                    pthread_mutex_lock(&mutex2);
                    delivery_man_2_delivery_time += shop_distance[order_shop];  
                    pthread_mutex_unlock(&mutex2);  

                    if (delivery_man_2_delivery_time > 30)
                    {
                        publish_message(client_fd,"Your delivery will take a long time, do you want to wait?"); 
                        // do{
                        printf("-----------------------------------\n");
                        printf("Costumer Deciding\n");
                        memset(received_message, 0, BUFSIZE);
                        message_callback(client_fd, received_message);
                        printf("Costumer Decided\n");
                        printf("-----------------------------------\n");
                        // }while((strcmp(received_message, "No\n") != 0) && (strcmp(received_message, "Yes\n") == 0));
                        if (strcmp(received_message, "No\n") == 0)
                        {
                            pthread_mutex_lock(&mutex2);
                            delivery_man_2_delivery_time -= shop_distance[order_shop];  
                            pthread_mutex_unlock(&mutex2);
                            break;
                        }else
                        {  
                            printf("delivery_man_2 go!!\n");
                            publish_message(client_fd, "Please wait a few minutes...");
                            pthread_mutex_lock(&mutex4);
                            //*****creitical section*****//
                            sleep(shop_distance[order_shop]);
                            delivery_man_2_customer += 1;
                            delivery_man_2_income += total_price; 

                            pthread_mutex_lock(&mutex2);
                            delivery_man_2_delivery_time -= shop_distance[order_shop];
                            pthread_mutex_unlock(&mutex2);
                            pthread_mutex_unlock(&mutex4);
                            //*****creitical section*****//

                            sprintf(msg, "Delivery has arrived and you need to pay %d$\n", total_price);  //當外送到時，顯示付款金額
                            publish_message(client_fd, msg);
                            break;
                        }
                    }
                    //**********deliver 2 delivering**********//
                    //increase that delivery man's total costumer, total income and sleep for delivering
                    printf("delivery_man_2 go!!\n");
                    publish_message(client_fd, "Please wait a few minutes...");

                    //*****creitical section*****//
                    pthread_mutex_lock(&mutex4);
                    sleep(shop_distance[order_shop]);
                    delivery_man_2_customer += 1;
                    delivery_man_2_income += total_price; 

                    pthread_mutex_lock(&mutex2);
                    delivery_man_2_delivery_time -= shop_distance[order_shop];
                    pthread_mutex_unlock(&mutex2);
                    pthread_mutex_unlock(&mutex4);
                    //*****creitical section*****//

                    sprintf(msg, "Delivery has arrived and you need to pay %d$\n", total_price);  //當外送到時，顯示付款金額
                    publish_message(client_fd, msg);
                    break;
                }
            }
        }
        //**********cancel**********//
        else if(strcmp(received_message, "cancel\n") == 0)
        {  
            break;
        }
        //**********order**********//
        else
        {   //order
            sscanf(received_message, "%s %s %d", order, meal, &count);

            //Determine the meals are in the same shop or not
            if (first_order)
            {  // Isn't the first order
                other_shop = select_shop(meal);
                if (other_shop == order_shop)  //new order shop = old order shop
                {     
                    char *new_ordered;
                    switch (other_shop) //calculate order's price
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
                    strcpy(ordered, new_ordered);  //determine the order is the same or not
                    free(new_ordered);  ///prevent interrupt
                }
                publish_message(client_fd, ordered);   
            }
            else   //first order
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
                sprintf(ordered, "%s %d", meal, count);
                publish_message(client_fd, ordered); 
            }
        }
    } 
    close(client_fd);
}
//**********Thread Function**********//

//**********Signal Handler**********//
void signalHandler(int signum) 
{
    //destroy mutex
    printf("signalHandler: Destory mutex\n");
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    pthread_mutex_destroy(&mutex3);
    pthread_mutex_destroy(&mutex4);
    printf("Main thread finished\n");
    exit(0);
}
//**********Signal Handler**********//

int main(int argc , char *argv[])
{   
    if (argc != 2)
        errexit("Usage: %s <PORT>\n", argv[0]); 

    //socket
    int server_fd, client_fd; 
    struct sockaddr_in addr_cln;
    socklen_t add_rlen = sizeof(addr_cln);

    //create socket and bind socket to port
    server_fd = passivesock(argv[1], "tcp", 50);  

    //signal handler
    signal(SIGINT, signalHandler);

    //initial mutex
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);
    pthread_mutex_init(&mutex3, NULL);
    pthread_mutex_init(&mutex4, NULL);

    int thread_nums = 0;
    int costumer = 0;
    pthread_t thread[NUMOFTHREADS];
    while (1)
    {
        //waiting for connection, accept connection from clients 
        client_fd = accept(server_fd, (struct sockaddr *)&addr_cln, &add_rlen);
        if (client_fd == -1)
            errexit("Error: accept()\n");        
        printf("Costomer #%d connected.\n",++costumer);

        //allocate client_fd pointer 
        int *client_fd_ptr = malloc(sizeof(int));
        if (client_fd_ptr == NULL)
            errexit("Memory allocation error\n");
        *client_fd_ptr = client_fd;

        int check_result = pthread_create(&thread[thread_nums++], NULL, get_order, (void *)client_fd_ptr); 
        checkResults("pthread_create()\n", check_result);

        pthread_detach(thread[thread_nums - 1]);
    }
    
    close(client_fd);

    //destroy mutex
    printf("Main thread clean up mutex\n");
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    pthread_mutex_destroy(&mutex3);
    printf("Main thread completed\n");

    return 0;
}