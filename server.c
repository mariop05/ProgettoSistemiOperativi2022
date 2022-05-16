#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include "fifo.h"
#include "err_exit.h"
#include "shared_memory.h"


int main() {

    char *myfifo1 = "/tmp/myfifo1";
    char *myfifo2 = "/tmp/myfifo2";
    int ricevo_file_sendme[1];
    char buffer_shd_memory[50];
    int key; // Chiave univoca per ShdMemory
    int shmid;
    int fd1; // Fifo 1
    int fd2; // Fifo 2


    // Creo due FIFO
    creaFifo(myfifo1);
    creaFifo(myfifo2);
    // Creo chieve univova per ShdMemory
    key = generateUniqueKey();
    //Creo la ShdMemory
    shmid = alloc_shared_memory(key, sizeof(buffer_shd_memory), IPC_CREAT | S_IRUSR | S_IWUSR);

    //Apro la fifo in lettura
    fd1 = apertura_fifo_lettura(myfifo1);
    //Leggo i dati dalla fifo
    lettura_fifo(fd1, ricevo_file_sendme);
    printf("numero file sendme_ : %d\n", ricevo_file_sendme[0]);


    // Rimozione delle FIFO
    rimozioneFifo(myfifo1);
    rimozioneFifo(myfifo2);

    //Rimozione ShdMemory
    remove_shared_memory(shmid);



    return 0;
}
