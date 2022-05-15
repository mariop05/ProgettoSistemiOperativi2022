#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/wait.h>
#include "err_exit.h"
#include "defines.h"
#include "fifo.h"

extern char **environ;

sigset_t mySet;
pid_t pid;
int count = 0; //Contiene il numero di file che iniziano con 'sendme_'
// The signal handler that will be used when the signal SIGINT
// is delivered to the process
void sigHandlerINT(int sig) {
    printf("Modifico la maschera\n");
    sigaddset(&mySet, SIGINT);
    sigaddset(&mySet, SIGUSR1);
    sigprocmask(SIG_SETMASK, &mySet, NULL);
    printf("Maschera modificata\n");
}

int main (int argc, char *argv[]) {
    pid = getpid();
    DIR *dp;
    int length; //Variabile che indica la lunghezza del pathname
    int flag = 0; // Variabile che indica se un file inizia con 'sendme_'
    char sendMe[] = "sendme_";
    char directory_corrente[SIZE];
    char file_massimi_sendme[100][250];
    int file_sendme[1];
    struct dirent *dentry;
    struct stat buffer;
    int i = 0;
    int fd; // Restituisce il valore dell'apertura della fifo
    char *myfifo1 = "/tmp/myfifo1";
    char *myfifo2 = "/tmp/myfifo2";

    printf("pid: %d\n", pid);
    // set of signals (N.B. it is not initialized!)
    // initialize mySet to contain all signals
    sigfillset(&mySet);
    // remove SIGINT and SIGUSR1 from mySet
    sigdelset(&mySet, SIGINT);
    sigdelset(&mySet, SIGUSR1);
    // blocking all signals but SIGINT
    sigprocmask(SIG_SETMASK, &mySet, NULL);


    // set the function sigHandler as handler for the signal SIGINT
    if (signal(SIGINT, sigHandlerINT) == SIG_ERR)
        ErrExit("change signal handler failed");

    //Attesa segnale
    pause();

   // argv[1] contiene il pathname inserito dall'utente
    create_directory(argv[1]);
    //printf("argv[1]: %s\n", argv[1]);
    // Get the username of name
    char *username = getenv("USER");
    if(username == NULL)
        username = "unknown";
    // Imposto la directory passata dall'utente
    if(chdir(argv[1]) == -1)
        ErrExit("error current directory\n");
    printf("Current directory: %s\n", getcwd(directory_corrente, SIZE));

    //Stampo la stringa Ciao USER...
    printf("Ciao %s ora inizio l'invio dei file dei file contenuti in %s\n", username, directory_corrente);

    //Apertura cartella
    dp = open_directory(argv[1]);
    // Trovo la lunghezza del pathname
    length = length_pathname(argv[1]);
    //printf("lunghezza path: %d\n", length);
    while((dentry = readdir(dp)) != NULL){
        if(dentry->d_type == DT_REG){
            //Creo il pathname complete
            create_pathname_format((struct dirent *) dentry, length, argv[1]);
            //printf("argv[1]: %s\n", argv[1]);
            //Verifico che il file inizi con 'sendme_'
            flag = equal_start_file(sendMe, (struct dirent *) dentry);
            //printf("flag: %d\n", flag);
            //Se i file iniziano con 'sendme_' stampo a video solo file che hanno un peso inferiore di 4KByte
            count = print_file(flag, buffer, argv[1], (struct dirent *) dentry) + count;

        }
        flag = 0;
    }
    file_sendme[0] = count;
    //Apertura fifo in scrittura
    fd = apertura_fifo_scrittura(myfifo1);
    //Scrittura su fifo
    scrittura_fifo(fd, file_sendme);

    // Generazione processi figli tanti quanti sono i file che iniziano con 'sendme_'


    //chiudo la cartella
    close_directory(dp);

    return 0;
}