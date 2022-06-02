#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <string.h>
#include "fifo.h"
#include "err_exit.h"
#include "shared_memory.h"
#include <sys/ipc.h>
#include <sys/msg.h>

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
    //Apro la fifo in lettura
    fd1 = apertura_fifo_lettura(myfifo1);
    //Leggo i dati dalla fifo
    lettura_fifo(fd1, ricevo_file_sendme);+

    printf("numero file sendme_ : %d\n", ricevo_file_sendme[0]);

    //shmat to attach to shared memory
    str = (char *) shmat(shmid, (void*)0, 0);
    //Invio la string start sulla shared memory
    strcpy(str, "start");
    printf("Send message: %s\n", str);


    //Create a key message queue
    key_message_queue = generate_unique_key_message_queue();
    printf("key message queue: %d\n", key_message_queue);
    msgid = msgget(key_message_queue, IPC_CREAT | S_IRUSR | S_IWUSR );
    printf("msgid: %d\n", msgid);
    message.mesg_type = 1;
    //Scrittura e invio messaggio tramite Message Queue
    strcpy(message.mesg_text, "Ciao sono message queue");
    msgsnd(msgid, &message, sizeof(message), 0);
    printf("Messaggio message queue: %s\n", message.mesg_text);
    // Rimozione delle FIFO
    rimozioneFifo(myfifo1);
    rimozioneFifo(myfifo2);

    //detach from shared memory
    free_shared_memory(str);



    return 0;
}
