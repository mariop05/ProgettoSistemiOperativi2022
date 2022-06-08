/// @file fifo.c
/// @brief Contiene l'implementazione delle funzioni
///         specifiche per la gestione delle FIFO.

#include <stdio.h>
#include "err_exit.h"
#include "fifo.h"
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

void creaFifo(char *myfifo){
    int result;
    result = mkfifo(myfifo, S_IRUSR | S_IWUSR);
    if(result == -1){
        ErrExit("error create FIFO\n");
    }
    else
        printf("Create a FIFO\n");
}
void rimozioneFifo(char *myfifo){

    int result;
    result =unlink(myfifo);
    if(result == -1)
        ErrExit("error removing FIFO\n");
    else
        printf("Removing FIFO\n");
}
int apertura_fifo_scrittura(char *myFifo){
    int fd = open(myFifo, O_WRONLY);
    if(fd == -1)
        ErrExit("error open fifo");
    return fd;
}
int apertura_fifo_lettura(char *myFifo){
    int fd = open(myFifo, O_RDONLY);
    if(fd == -1)
        ErrExit("error open read fifo");
    return fd;
}
void scrittura_fifo(int fd, int file_sendme[]){
    if(write(fd, file_sendme, sizeof(int)) == -1){
        ErrExit("error write");
    }
}
void lettura_fifo(int fd, int numero[]){
    if(read(fd, numero, sizeof(int)) == -1){
        ErrExit("error write");
    }
}
void lettura_fifo_caratteri(int fd, char array[]){
    if(read(fd, array, sizeof(char)) == -1){
        ErrExit("error write");
    }
}
void scrittura_fifo_caratteri(int fd, char array[]){
    if(write(fd, array, sizeof(char)) == -1){
        ErrExit("error write");
    }
}