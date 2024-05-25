//standard library
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

//shared memory and signals
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

//Data Struct
typedef struct {
    int guess;
    char result[8];
}data;

#define errexit(format,arg...) exit(printf(format,##arg))

//signal flag
sig_atomic_t flag = 0;

//guess process
pid_t guess_pid;

//shared memory id
int shmid = 0;

//guess handler
void guess_handler (int signo, siginfo_t *info, void *context)
{
    guess_pid = info->si_pid;
    flag = 1;
}

//SIGINT Handler
void CtrlCHandler(int sig_num)
{
    // Destroy the share memory segment 
    printf("[game.c]: Destroy the share memory.\n");
    if (shmctl(shmid, IPC_RMID, NULL) < 0)
    {
        errexit("Failed removing share memory.\n");
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    //game parameters
    int answer = 0, guess = 0;
    const char* bigger = "bigger";
    const char* smaller = "smaller";
    const char* bingo = "bingo";

    //shared memory key
    data *sh_data, *shm;
    key_t key;
    
    //signal action
    struct sigaction game_action;

    if (argc != 3)
    {   
        errexit("Usage: %s <key> <guess>\n", argv[0]);
    }

    //setting game parameters
    key = atoi(argv[1]);
    answer = atoi(argv[2]);

    printf("[game] Game PID: %d\n", getpid());

    //Catching Signals
    memset(&game_action, 0, sizeof (struct sigaction));
    game_action.sa_flags = SA_SIGINFO;
    game_action.sa_sigaction = &guess_handler;
    sigaction(SIGUSR1, &game_action, NULL);
    signal(SIGINT, CtrlCHandler);

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
    sh_data->guess = 0;

    //Dealing Guess Atempt
    while(1)
    {
        if(flag == 1)
        {
            flag = 0;
            guess = sh_data->guess;
            memset(sh_data->result, 0, sizeof(sh_data->result));
            if(guess == answer) //bingo
                strcpy(sh_data->result, bingo);
            else if(guess > answer) //smaller 
                strcpy(sh_data->result, smaller);  
            else if(guess < answer) //bigger
                strcpy(sh_data->result, bigger);
            printf("[game] Guess %d, %s\n", guess, sh_data->result);

            //SIGUSR1
            kill(guess_pid, SIGUSR1);    
        }
    }

    return 0;
}