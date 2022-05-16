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
    pid_t child;
    DIR *dp;
    int length; //Variabile che indica la lunghezza del pathname
    int flag = 0; // Variabile che indica se un file inizia con 'sendme_'
    int i = -1, j, k;
    int open_file;
    char sendMe[] = "sendme_";
    char directory_corrente[SIZE];
    char file_pathname_sendme[100][250];
    int file_sendme[1];
    struct dirent *dentry;
    int fd1; // Restituisce il valore dell'apertura della fifo 1
    int fd2; // Restituisce il valore dell'apertura della fifo 2
    char *myfifo1 = "/tmp/myfifo1";\
    char *myfifo2 = "/tmp/myfifo2";
    off_t caratteri;

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
            //Verifico che il file inizi con 'sendme_' e restituisce il numero totale di file che iniziano con 'sendme_'
            //e una dimensione minore di 4KByte
            count = equal_start_file(sendMe, (struct dirent *) dentry, flag, argv[1]);
            if(count != 0){
                i++;
                //printf("i: %d\n", i);
                //printf("Stampo la matrice: ");
                for(j = i; j <= count;j++){
                    for (k = 0; dentry->d_name[k] != '\0'; k++) {
                        file_pathname_sendme[j][k] = dentry->d_name[k];
                        //printf("%c", file_pathname_sendme[j][k]);
                    }
                    //printf("\n");
                }
            }
            count = count + 1;
//          printf("argv[1]: %s\n", argv[1]);
        }
        flag = 0;
    }

//    //Stampo la matrice con i pathname
//    printf("Stampo matrice:\n");
//    for(j = 0; j <= count; j++){
//        for (int k = 0; file_pathname_sendme[j][k] != '\0'; k++) {
//            printf("%c", file_pathname_sendme[j][k]);
//        }
//        printf("\n");
//    }

    printf("count: %d\n", count);
    file_sendme[0] = count;
    //Apertura fifo in scrittura
    fd1 = apertura_fifo_scrittura(myfifo1);
    //Scrittura su fifo
    scrittura_fifo(fd1, file_sendme);

    printf("count: %d\n", count);
    //Generazione processi figli tanti quanti sono i file che iniziano con 'sendme_'
    printf("Creazione processi figli\n");
    i = 0;
    for(i = 0; i < count; ++i) {
        printf("count for: %d\n", count);
        printf("i: %d\n", i);
        child = fork();
        printf("child: %d\n", child);
        if (child == -1)
            ErrExit("Error fork");
        else if (child == 0) {
            //Apertura file
            open_file = open(file_pathname_sendme[i], O_RDWR);
            if (open_file == -1)
                ErrExit("error open file");
            //printf("Aperto file: %s\n", file_pathname_sendme[i]);
            //Salvo quanti caratteri ha un file
            caratteri = conta_caratteri(open_file);

            if ((caratteri % 4) == 0) {
                // Inizializzo i 4 array che conterranno i caratteri del file
                char file1[caratteri / 4];
                char file2[caratteri / 4];
                char file3[caratteri / 4];
                char file4[caratteri / 4];

                //Inserisco 1/4 nell'array file1
                riempo_array_pari(file1, caratteri / 4, open_file);
                //Inserisco 1/4 nell'array file2
                riempo_array_pari(file2, caratteri / 4, open_file);
                //Inserisco 1/4 nell'array file1/
                riempo_array_pari(file3, caratteri / 4, open_file);
                // /Inserisco 1/4 nell'array file1
                riempo_array_pari(file4, caratteri / 4, open_file);

                //Stampo array per verifica
                //1/4 file 1
                stampo_array_divisione_file(file1, caratteri / 4);
                stampo_array_divisione_file(file2, caratteri / 4);
                stampo_array_divisione_file(file3, caratteri / 4);
                stampo_array_divisione_file(file4, caratteri / 4);
            }
                // File contenente un numero di carateri dispari
            else {
                char file1[caratteri / 4];
                char file2[caratteri / 4];
                char file3[caratteri / 4];
                char file4[caratteri % 4];

                //Inserisco 1/4 nell'array file1
                riempo_array_pari(file1, caratteri / 4, open_file);
                //Inserisco 1/4 nell'array file2
                riempo_array_pari(file2, caratteri / 4, open_file);
                //Inserisco 1/4 nell'array file1/
                riempo_array_pari(file3, caratteri / 4, open_file);
                // /Inserisco 1/4 nell'array file1
                riempo_array_pari(file4, caratteri % 4, open_file);

                //Stampo array per verifica
                //1/4 file 1
                stampo_array_divisione_file(file1, caratteri / 4);
                stampo_array_divisione_file(file2, caratteri / 4);
                stampo_array_divisione_file(file3, caratteri / 4);
                stampo_array_divisione_file(file4, caratteri / 4);
            }
            exit(0);
//            printf("caratteri: %ld\n", caratteri);

        } else {
            //Sono nel processo genitore
            if (wait(NULL) == -1)
                ErrExit("error wait");
        }
    }
    //chiudo la cartella
    close_directory(dp);

    return 0;
}