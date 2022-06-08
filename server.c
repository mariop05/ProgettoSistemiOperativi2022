#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <string.h>
#include "fifo.h"
#include "err_exit.h"
#include "shared_memory.h"
#include "semaphore.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>

#define REQUEST 0
#define DATA_READY 1

struct message{
    char prima_parte[250];
    char seconda_parte[250];
    char terza_parte[250];
    char quarta_parte[250];
    int pid;
    char pathname[250];

}message_send[100];

void decrement_semaphore (int sem_id)
{
    struct sembuf sem_op;

    sem_op.sem_num  = 0;
    sem_op.sem_op   = -1;
    sem_op.sem_flg = 0;

    semop(sem_id, &sem_op, 1);
}

void wait_semaphore (int sem_id)
{
    struct sembuf sem_op;

    sem_op.sem_num  = 0;
    sem_op.sem_op   = 0;
    sem_op.sem_flg = 0;

    semop(sem_id, &sem_op, 1);
}


// structure for message queue
struct mesg_buffer {
    long mesg_type;
    char mesg_text[100];
} message;
int main() {

    char *myfifo1 = "/tmp/myfifo1";
    char *myfifo2 = "/tmp/myfifo2";
    char *pathname_file_shared_memory = "/tmp/pathname.txt";
    char *pathname_file_message_queue = "/tmp/pathname_message_queue_server.txt";
    char *str;
    int ricevo_file_sendme[1];
    char buffer_shd_memory[50];
    key_t key; // Chiave univoca per ShdMemory
    key_t key_message_queue;
    int shmid;
    int msgid;
    int fd1; // Fifo 1
    int fd2; // Fifo 2


    // Creo due FIFO
    creaFifo(myfifo1);
    creaFifo(myfifo2);

    /* --------- Creo chieve univoca per ShdMemory ------*/
    key = generateUniqueKey();
    printf("key server: %d\n", key);
    shmid = alloc_shared_memory(key, 1024, IPC_CREAT | S_IRUSR | S_IWUSR);
    printf("shmid: %d\n", shmid);

    //Create a key message queue
    key_message_queue = generate_unique_key_message_queue();
    printf("key message queue: %d\n", key_message_queue);
    msgid = msgget(key_message_queue, IPC_CREAT | S_IRUSR | S_IWUSR );
    printf("msgid: %d\n", msgid);
    message.mesg_type = 1;

    //Creo semaforo
    int semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
    if(semid == -1)
        ErrExit("error create semaphore");

    printf("Apertura fifo1\n");
    //Apro la fifo in lettura
    fd1 = apertura_fifo_lettura(myfifo1);
    //Leggo i dati dalla fifo
    lettura_fifo(fd1, ricevo_file_sendme);


    printf("numero file sendme_ : %d\n", ricevo_file_sendme[0]);
    if(ricevo_file_sendme[0] > 100)
        ErrExit("Ci sono più di 100 file sendme_");

    // Rimozione delle FIFO
    //shmat to attach to shared memory
    str = (char *) shmat(shmid, (void*)0, 0);
    //Invio la string start sulla shared memory
    strcpy(str, "start");
    printf("Send message: %s\n", str);

    //Scrittura e invio messaggio tramite Message Queue
    strcpy(message.mesg_text, "Ciao sono message queue");
    printf("copiato messaggio\n");
    msgsnd(msgid, &message, sizeof(message), 0);
    //unlock client
    printf("Messaggio message queue: %s\n", message.mesg_text);

    for(int i = 0; i < ricevo_file_sendme[0]; i++) {


        //Lettura fifo 1
        fd1 = apertura_fifo_lettura(myfifo1);
        lettura_fifo_caratteri(fd1, message_send[i].prima_parte);
        printf("fifo 1: %s\n", message_send[i].prima_parte);
        close(fd1);
        //Lettura fifo 2
        fd2 = apertura_fifo_lettura(myfifo2);
        lettura_fifo_caratteri(fd2, message_send[i].seconda_parte);
        printf("fifo 2: %s\n", message_send[i].seconda_parte);
        close(fd2);

    }




    // Rimozione delle FIFO
    rimozioneFifo(myfifo1);
    rimozioneFifo(myfifo2);

    //detach from shared memory
    free_shared_memory(str);
    //Remove shared memory
    if (semctl(semid, 0 /*ignored*/, IPC_RMID, NULL) == -1)
        ErrExit("semctl IPC_RMID failed");


    return 0;
}
