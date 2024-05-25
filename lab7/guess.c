//standard library
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

//shared memory, signals and timers
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>

//Data Struct
typedef struct {
    int guess;
    char result[8];
}data;

#define errexit(format,arg...) exit(printf(format,##arg))

//signal flag
sig_atomic_t flag = 0;

//SIGUSR1_handler
void SIGUSR1_handler (int signal_number)
{
    flag = 1;
}

int main(int argc, char *argv[])
{
    //game parameters
    int shmid, guess = 0, bound = 0, result;
    const char* bigger = "bigger";
    const char* smaller = "smaller";
    const char* bingo = "bingo";

    //game process
    pid_t game_pid;
    
    //shared memory key
    data *sh_data, *shm;
    key_t key;
    
    //signal action
    struct sigaction guess_action;

    //timer
    struct timespec req, rem;

    if (argc != 4)
    {
        errexit("Usage: %s <key> <upper_bound> <pid>\n", argv[0]);
    }
    key = atoi(argv[1]);
    bound = atoi(argv[2]);
    game_pid = atoi(argv[3]);

    // set the sleep time to 1 sec 
    memset(&req, 0, sizeof(struct timespec));
    req.tv_sec = 1;
    req.tv_nsec = 0;

    //register SIGUSR1_handler to SIGUSR1
    memset(&guess_action, 0, sizeof (struct sigaction));
    guess_action.sa_handler = SIGUSR1_handler;
    sigaction (SIGUSR1, &guess_action, NULL);

    //Create the shared memory
    shmid = shmget(key, sizeof(data), IPC_CREAT | 0666);

    //Attach the segment to the data space
    if ((shm = shmat(shmid, NULL, 0)) == (data *) -1) 
    {
        perror("shmat");
        exit(1);
    }
    printf("[Info] Create and attach the share memory.\n");
    sh_data = shm;

    //Writing Guess in Shared Memory
    while(1)
    {
        result = nanosleep(&req, &rem); 

        if (result == -1)
            nanosleep(&rem, &rem);

        if(flag == 1)
        {
            if (strstr(sh_data->result, bigger) != NULL) //bigger
            {
                guess = (bound - guess)/2 + guess;
            }
            else if (strstr(sh_data->result, smaller) != NULL) //smaller
            {
                bound = guess;
                guess /= 2;
            }
            else if (strstr(sh_data->result, bingo) != NULL) //bingo
                break;
            flag = 0;
        }
        printf("[game] Guess: %d\n", guess);
        sh_data->guess = guess;
        kill(game_pid, SIGUSR1); 
    } 

    // Detach the share memory segment 
    shmdt(shm);
    return 0;
}