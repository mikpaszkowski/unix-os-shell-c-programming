#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>
#include <wait.h>
#include <unistd.h>

#define NUM_OF_PHILOSOPHERS 5
#define LEFT_FORK PHILOSOPHER_ID
#define RIGHT_FORK (PHILOSOPHER_ID + 1) % NUM_OF_PHILOSOPHERS
#define NUM_OF_MEALS 2

int PHILOSHPERS_PIDS[NUM_OF_PHILOSOPHERS];
static int SEMAPHORE_ID;
int CURRENT_PHILOSOPHER_ID;


void initialize_philosphers(){

    // system call create new set of semaphors
    // return a system V semaphore set identifier associated with the argument key
    // key_t type must have the value IPC_PRIVATE to create new set of semaphorses in number of NUM_OF_PHILOSPHERS
    // in case of no existsing semaphore is asscociated with the key and IPC_CREATE is specified in semflg, then the new samaphore is created
    // 0666 - operation permission specified by bits in octal notation
    SEMAPHORE_ID = semget(IPC_PRIVATE, NUM_OF_PHILOSOPHERS, 0666); 

    // if the semaphore is created successfully, then nonnegative integer is returned, otherwise -1 is returned
    if(SEMAPHORE_ID == -1){
        printf("Create semaphore failure\n");
        exit(1);
    }

    for(int i = 0; i < NUM_OF_PHILOSOPHERS; i++){

        //The semctl() function provides a variety of semaphore control operations as specified by cmd
        //When this command is successfully executed, the semadj value corresponding to the specified semaphore in all processes is cleared
        //setting semaphore value (semval) to 1 - every forks are free initially 
        if(semctl(SEMAPHORE_ID, i, SETVAL, 1) == -1){
            printf("Cannot set semaphore.\n");
            exit(1);
        }
    }
}

//termination of child process
void terminateChildProcess(int i){
    for(int j = 0; j < i; j++){
        //sending SIGTERM - termination signal to the given process
        kill(PHILOSHPERS_PIDS[j], SIGTERM);
    }
}


void grab_forks(int left_fork_id){

    //initialization of LEFT_FORK and RIGHT_FORK
    int PHILOSOPHER_ID = left_fork_id;

    //If semval is less than the absolute value of sem_op and (sem_flg&IPC_NOWAIT) is 0, semop() will increment the semncnt associated with the specified semaphore
    /*
         The elements of this structure are of type struct sembuf, containing the following members:

           unsigned short sem_num;  semaphore number
           short          sem_op;   semaphore operation
           short          sem_flg;  operation flags 
    */

   //If sem_op is a negative integer and the calling process has alter permission
   struct sembuf SEM_OPERATION[] = {
       { .sem_num = LEFT_FORK, .sem_op = -1, .sem_flg = 0 },
       { .sem_num = RIGHT_FORK, .sem_op = -1, .sem_flg = 0 }
   };


    // Each of the nsops elements in the array pointed to by sops is a structure that specifies an operation to be
    // performed on a single semaphore.
    if(semop(SEMAPHORE_ID, SEM_OPERATION, 2) == -1){
        printf("ERROR in grap_forks method: failure occured in semop");
        exit(1);
    }
}

void put_away_forks(int left_fork_id){
    //same as grap_forks
    int PHILOSOPHER_ID = left_fork_id;

   //If sem_op is a positive integer, the operation adds this value to the semaphore value (semval).
    struct sembuf SEM_OPERATION[] = {
        { .sem_num = LEFT_FORK, .sem_op = 1, .sem_flg = 0 },
        { .sem_num = RIGHT_FORK, .sem_op = 1, .sem_flg = 0 }
    };

    if(semop(SEMAPHORE_ID, SEM_OPERATION, 2) == -1){
        printf("ERROR in put_away_forks method: failure occured in semop");
        exit(1);
    }

    printf("Philosopher[%d] put LEFT_FORK[%d] and RIGHT_FORK[%d] down\n", RIGHT_FORK, LEFT_FORK, RIGHT_FORK);
}

void eat(int num){

    int PHILOSOPHER_ID = num;
    sleep(2);

    printf("Philosopher[%d] takes LEFT_FORK[%d] and RIGHT_FORK[%d]\n", RIGHT_FORK, LEFT_FORK, RIGHT_FORK);
    printf("Philosopher[%d] starts eating\n", RIGHT_FORK);
}



void *philosopher(){


    int PHILOSOPHER_ID = CURRENT_PHILOSOPHER_ID; 

    for (int i = 0; i < NUM_OF_MEALS; i++) {
        printf("Philosopher[%d] is THINKING\n", RIGHT_FORK);
		sleep(1);
		grab_forks(PHILOSOPHER_ID);
		eat(PHILOSOPHER_ID);
		sleep(1);
		put_away_forks(PHILOSOPHER_ID);
	}
}

void terminateOrRun(pid_t pid, int i){

        // if pid is negative means that error occured
        if(pid < 0){

            printf("Create child process failure\n");
            terminateChildProcess(i);
            exit(1);

        // when pid == 0 the child process created successfully and 0 because returned in the child
        }else if(pid == 0){

            PHILOSHPERS_PIDS[i] = getpid();
            CURRENT_PHILOSOPHER_ID = i;
            philosopher();
            exit(0);
        }
}


int main(){

    //initialize philosphers
    initialize_philosphers();

    pid_t chil_pid;
    int status;

    //loop for running the simulation on every philosophers
    for(int i = 0; i < NUM_OF_PHILOSOPHERS; i++){

        chil_pid = fork();

        terminateOrRun(chil_pid, i);
    }

    while (1) {
        //The wait() system call suspends execution of the calling thread
        // until one of its children terminates
        // child_pid == -1 meaning to wait for any child process
		chil_pid = wait(&status);
		if (chil_pid == -1)
			break;

	}

    //Immediately remove the semaphore set, awakening all processes blocked in semop(2) calls on the set 
    // semnum set to 0 but it is ignored for cmd equals to IPC_RMID
    if (semctl(SEMAPHORE_ID, 0, IPC_RMID) == -1) {
        printf("ERROR: removing semaphore failure\n");
        exit(1);
    }
}
