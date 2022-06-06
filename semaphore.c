/// @file semaphore.c
/// @brief Contiene l'implementazione delle funzioni
///         specifiche per la gestione dei SEMAFORI.

#include "err_exit.h"
#include "semaphore.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>


int creazione_semaforo(key_t key, int numero_semafori, int flag){
    int semid = semget(key, numero_semafori, flag | S_IRUSR | S_IWUSR);

    if(semid == -1)
        ErrExit("Errore creazione semaforo");

    return semid;
}
void remove_semaforo(int semid){
    if(semctl(semid, 0, IPC_RMID, NULL) == -1){
        ErrExit("Errore rimozione semaforo");
    }
}
void semOp(int semid, unsigned short sem_num, short sem_op){
    struct sembuf sop = {.sem_num = sem_num, .sem_op = sem_op, .sem_flg = 0};
    printf("semop id: %d\n", semop(semid, &sop, 1));
    if(semop(semid, &sop, 1) == -1)
        ErrExit("semop failed");
}