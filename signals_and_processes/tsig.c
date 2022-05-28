#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>

#define NUM_CHILD 10    //number of childs to be created
#define WITH_SIGNALS    // WITH_SIGNALS preprocessor symbol

pid_t pidsOfCreatedChilds[NUM_CHILD];   //array containing children processes



#ifdef WITH_SIGNALS //WITH_SIGNALS  definition will be preprocessed from #ifdef to #endif

static bool keyboardInterruptOccurence = false;     // bool flag for keyboard interrupt occurence 
                                                    // when interrupted then flag is set to true
                                                    //static field to refer to the same place in memory
 void interruptSignalHandler() {                                    // handling interrupt signal - SIGINT
        printf("parent[%d]: keyboard interrupt signal\n", getpid());
        keyboardInterruptOccurence = true;                          // setting flag to true when signal received                  
    }

    void terminationSignalHandler() {                               // message when process interrupted
        printf("child[%d]: received SIGTERM signal, terminating\n", getpid());
    }

#endif

void terminateProcess(int childNum){                // method for terminating already created processes
    for(int i = 0; i < childNum; i++){
        printf("parent[%d]: child process terminated, sending SIGTERM to child[%d]\n", getpid(), pidsOfCreatedChilds[i]);
        kill(pidsOfCreatedChilds[i], SIGTERM);      // terminating specific process of array of children
                                                    // sending SIGTERM to process number PID from array
        exit(1);
    }
}

void childProcessAlgorithm() {                      // child process algorithm
    
    #ifdef WITH_SIGNALS
        signal(SIGINT, SIG_IGN);                    // ignoring interrupt signal by setting ignore signal on signal interrupt
        signal(SIGTERM, terminationSignalHandler);    // passing own created termination handler when SIGTERM signal send
    #endif

    printf("parent[%d]: created new child[%d]\n", getppid(), getpid());
    sleep(10);
    printf("child[%d]: execution completion\n", getpid());
}

//   Call in a loop the wait() function, until receiving from the system
//   information that there are no more processes to be synchronized with the
//   parent one. 
//   Print a message that there are no more child processes. In this
//   loop do count child processes terminations and, at the very end of the
//   parent process, print a message with the number of just received child
//   processes exit codes.

void infoMessage() {            

    pid_t childPid;
    int processStatus, numOfStatusOne = 0, numOfStatusZero = 0;

    while(1){

        childPid = wait(&processStatus);        //waiting for child to die, return -1 when failure or process PID
        
        if(childPid == -1){                     //checking failure
            printf("parent[%d]: there are no more child processes to be synchronized\n", getpid());
            break;
        }
        
        if(WIFEXITED(processStatus)){           //checking if child was terminated without problems
            printf("child[%d]: exit status [%d]\n", childPid, WEXITSTATUS(processStatus));
        }
        
        if (WEXITSTATUS(processStatus) == 1){   // counting child processes with code 1
            numOfStatusOne++;
        }else if(WEXITSTATUS(processStatus) == 0){ // counting child processes with code 0
            numOfStatusZero++;
        }

    }
    printf("Child process terminations: %d\n", numOfStatusOne + numOfStatusZero);
    printf("Child processes terminates with status code: 0 (count: %d), 1 (count: %d)\n", numOfStatusZero, numOfStatusOne);
}

void createChild(int childNum) {
    /**
     * On success, the PID of the child process is returned in the 
     * parent, and 0 is returned in the child.  On failure, -1 is 
     * returned in the parent, no child process is created, and errno is
     * set to indicate the error.
     */
      
    pid_t childPidNum = fork();                     // creating new child process

    if(childPidNum > 0){            
        pidsOfCreatedChilds[childNum] = childPidNum; //   
        printf("parent[%d]: created child[%d]\n", getpid(), childPidNum);
    }
    else if(childPidNum == 0){
        childProcessAlgorithm();
        exit(0);
    }else {
        printf("child: error | cannot create new child");
        terminateProcess(childNum);
        exit(1);
    }
}

int main() {

    #ifdef WITH_SIGNALS                         // force ignoring of all signals with the signal() (or sigaction()) but
                                                 // after that at once restore the default handler for SIGCHLD signal
        for(int i = 1; i < _NSIG; i++){         //_NSIG is the maximal number (real-time maximum) of signals
            signal(i, SIG_IGN);                 // ignoring all signals by SIG_IGN - signal ignored
        }       
        signal(SIGCHLD, SIG_DFL);               // default handler SIG_DFL for SIGCHILD signal
        signal(SIGINT, interruptSignalHandler); // customer interrupting handler for SIGINT
    #endif

    printf("\nparent[%d]\n", getpid());         //printing parent's PID

    for(int i = 0; i < NUM_CHILD; i++){         // loop for creating child processes
        createChild(i);                         // creating child
        sleep(1);           

        #ifdef WITH_SIGNALS
            if(keyboardInterruptOccurence){     //checking for keyboard interrupt signal
                terminateProcess(i);            // if so, terminating this process
                break;
            }
        #endif

    }

    infoMessage();  

    ///the old service handlers of all signals should be restored.

    #ifdef WITH_SIGNALS
        for(int i = 0; i < _NSIG; i++){
            signal(i, SIG_DFL);     // 
        }
    #endif

}