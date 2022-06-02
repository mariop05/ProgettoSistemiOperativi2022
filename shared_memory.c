/// @file shared_memory.c
/// @brief Contiene l'implementazione delle funzioni
///         specifiche per la gestione della MEMORIA CONDIVISA.

#include <fcntl.h>
#include <unistd.h>
#include "err_exit.h"
#include "shared_memory.h"
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>

// ftok generate a unique key
int generateUniqueKey(){
    return ftok("/tmp/shmfile", 65);
}
// attach the shared memory
int alloc_shared_memory(key_t key, size_t size, int shmflg){
    int shmid = shmget(key, size, shmflg);
    return shmid;
}
void *get_shared_memory(int shmid, int shmflg){
    //attach the shared memory
    void *ptr_sh = shmat(shmid, (void*) 0, shmflg);
    if(ptr_sh == (void*)-1)
        ErrExit("shmat failed");
    return ptr_sh;
}
// detach the shared memory segments
void free_shared_memory(char *str){
    if(shmdt(str) == -1){
        ErrExit("shmdt failed");
    }
}
void remove_shared_memory(int shmid){
    char *pathname = "/tmp/pathname.txt";
    //delete the shared memory segment
    if(shmctl(shmid, IPC_RMID, NULL) == -1){
        ErrExit("shmctl failed");
    }
    if(unlink("/tmp/shmfile") == -1)
        ErrExit("Error delete file");
}
int generate_unique_key_message_queue(){
    return ftok("/tmp/msgfile.txt", 1);
}
int create_message_queue(key_t key){
    int msgid = msgget(key, S_IRUSR | S_IWUSR);
    if(msgid == -1)
        ErrExit("error create message queue");
    return msgid;
}