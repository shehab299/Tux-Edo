#include "Includes/defs.h"

int shmid;

void cleanup(int signum)
{
    shmctl(shmid, IPC_RMID, NULL);
    printf("Clock terminating!\n");
    exit(0);
}

int main(int argc, char * argv[])
{
    printf("Clock starting\n");
    signal(SIGINT, cleanup);
    int clk = 0;

    shmid = shmget(SHKEY, 4, IPC_CREAT | 0644);     

    if ((long)shmid == -1)
    {
        perror("Error in creating shm!");
        exit(-1);
    }

    int * shmaddr = (int *) shmat(shmid, (void *)0, 0);

    if ((long)shmaddr == -1)
    {
        perror("Error in attaching the shm in clock!");
        exit(-1);
    }

    *shmaddr = clk; 

    while (1)
    {
        sleep(1);
        (*shmaddr)++;
    }

}
