#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "err_exit.h"
#include "defines.h"
#include "fifo.h"
#include "shared_memory.h"
#include "semaphore.h"

#define REQUEST 0
#define DATA_READY 1

struct mesg_buffer{
    long mesg_type;
    char mesg_text[250];
} message;
union semun {
    int val; /* Value for SETVAL */
    struct semid_ds *buf; /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* Array for GETALL, SETALL */
};
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

extern char **environ;

sigset_t mySet;
pid_t pid;
int count = 0;//Contiene il numero di file che iniziano con 'sendme_'
int count_sendme = 0;
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
    int i = 0, j, k;
    int open_file;
    int shmid;
    int msgid;
    int semaforo_di_supporto;
    int semaforo_principale;
    int semaforo;
    key_t key;
    key_t key_message_queue;
    char *username;
    char *str;
    char sendMe[] = "sendme_";
    char *pathname_file_shared_memory = "/tmp/pathname.txt";
    char *pathname_file_message_queue = "/tmp/pathname_message_queue_client.txt";
    char directory_corrente[SIZE];
    char file_pathname_sendme[100][250];
    int file_sendme[1];
    struct dirent *dentry;
    int fd1; // Restituisce il valore dell'apertura della fifo 1
    int fd2; // Restituisce il valore dell'apertura della fifo 2
    char *myfifo1 = "/tmp/myfifo1";\
    char *myfifo2 = "/tmp/myfifo2";

    union semun arg;
    unsigned short values[] = {0};

    union semun arg_main;
    unsigned short values_main[50] = {0, 0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,00,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0, 0, 0, 1};

    off_t caratteri;

    //Creo la SHARED MEMORY
    //ftok to generate a unique key
    key = generateUniqueKey();
    printf("key: %d\n", key);
    // Alloco segmento memoria condivisa
    shmid = alloc_shared_memory(key, 1024, S_IRUSR | S_IWUSR);
    printf("shmid: %d\n", shmid);

    //Creo la MESSAGE QUEUE
    //Creo coda di messaggi

    key_message_queue = generate_unique_key_message_queue();
    printf("key message queue: %d\n", key_message_queue);
    //Create a message queue
    msgid = create_message_queue(key_message_queue);
    printf("msgid: %d\n", msgid);
    message.mesg_type = 1;

    //INIZIALIZZO IL SET DI SEGNALI
    printf("pid: %d\n", pid);
    // set of signals (N.B. it is not initialized!)
    // initialize mySet to contain all signals
    sigfillset(&mySet);
    // remove SIGINT and SIGUSR1 from mySet
    sigdelset(&mySet, SIGINT);
    sigdelset(&mySet, SIGUSR1);
    // blocking all signals but SIGINT
    sigprocmask(SIG_SETMASK, &mySet, NULL);

    //CREO SET SEMAFORI
    semaforo_di_supporto = creazione_semaforo(IPC_PRIVATE, 1, IPC_CREAT);
    printf("semaforo di supporto: %d\n", semaforo_di_supporto);
    semaforo_principale = creazione_semaforo(76, 50, IPC_CREAT | S_IRUSR | S_IWUSR);
    printf("semaforo principale: %d\n");
    arg_main.array = values_main;
    if(semctl(semaforo_principale, 0, SETALL, values_main) == -1)
        ErrExit("semctl FAILED");
    // set the function sigHandler as handler for the signal SIGINT
    if (signal(SIGINT, sigHandlerINT) == SIG_ERR)
        ErrExit("change signal handler failed");

    //Attesa segnale
    pause();

   // argv[1] contiene il pathname inserito dall'utente
    create_directory(argv[1]);
    //printf("argv[1]: %s\n", argv[1]);
    // Get the username of name
    username = getenv("USER");
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
            //printf("Count: %d\n", count);
            if(count != 0){
                printf("i: %d\n", i);
                //printf("Stampo la matrice:");
                    for (k = 0; dentry->d_name[k] != '\0'; k++) {
                        file_pathname_sendme[i][k] = dentry->d_name[k];
                        //printf("%c", file_pathname_sendme[i][k]);
//                        printf("dentry->d_name: %c\n", dentry->d_name[k]);
                    }
                printf("\n");
                i++;
                count_sendme = count_sendme + count;
                //printf("count_sendme: %d\n", count_sendme);
            }

//          printf("argv[1]: %s\n", argv[1]);
        }
        flag = 0;
    }
//    printf("count: %d\n", count_sendme);
    file_sendme[0] = count_sendme ;
//    printf("file_sendme[0]: %d\n", file_sendme[0]);
    if(file_sendme[0] > 100)
        ErrExit("Ci sono più di 100 file sendme_");

    //Apertura fifo in scrittura e scrittura su fifo
    fd1 = apertura_fifo_scrittura(myfifo1);
    scrittura_fifo(fd1, file_sendme);
    close(fd1);
//    printf("Scrittura su fifo terminata\n");
//    printf("prima chiamata semaforo\n");

    sleep(2);

    str = (char *) shmat(shmid, (void*) 0, 0);

    printf("Date read: %s\n", str);
    //Attacco il segmento di memoria condivisa
//    printf("Effettuato attacco memoria condivisa\n");


    // msgrcv to receive message
    printf("msgrcv: %ld\n", msgrcv(msgid, &message, sizeof(message), 0, 0));
    printf("Received message queue: %s\n", message.mesg_text);




    //printf("count: %d\n", count);
    //Generazione processi figli tanti quanti sono i file che iniziano con 'sendme_'
    printf("Creazione processi figli\n");
    for(i = 0; i < count_sendme; i++) {
//        printf("count for: %d\n", count);
        printf("i: %d\n", i);
        child = fork();
//        printf("child: %d\n", child);
        if (child == -1)
            ErrExit("Error fork");
        else if (child == 0) {
            printf("----------- Sono il processo figlio PID: %d------------------------\n", getpid());
            //Apertura file
            message_send[i].pid = getpid();
            open_file = open(file_pathname_sendme[i], O_RDWR);
            printf("file_pathname_sendme[%d]: %s\n", i, file_pathname_sendme[i]);
            if (open_file == -1) {
                printf("Errore apertura file\n");
                ErrExit("error open file");
            }
            printf("Aperto file: %s\n", file_pathname_sendme[i]);
            //Salvo quanti caratteri ha un file
            caratteri = conta_caratteri(open_file);
            semOp(semaforo_principale, (unsigned short ) i, 1);
            //printf("caratteri: %ld\n", caratteri);
            if ((caratteri % 4) == 0) {

                printf("CARATTERI PARI\n");

                // Inizializzo i 4 array che conterranno i caratteri del file
                char file1[caratteri/4];
                char file2[caratteri/4];
                char file3[caratteri/4];
                char file4[caratteri/4];



                //Inserisco 1/4 nell'array file1
                riempo_array_pari(file1, caratteri/4, open_file);
                strcpy(message_send[i].prima_parte, file1);
                printf("Message_send prima parte: %s\n", message_send[i].prima_parte);
                printf("Apertura fifo 1\n");
                fd1 = apertura_fifo_scrittura(myfifo1);
                scrittura_fifo_caratteri(fd1, message_send[i].prima_parte);
                printf("Chiusura fifo 1\n");
                close(fd1);

                //Inserisco 1/4 nell'array file2
                riempo_array_pari(file2, caratteri/4, open_file);
                strcpy(message_send[i].seconda_parte, file2);
                printf("Message_send seconda parte: %s\n", message_send[i].seconda_parte);
                printf("Apertura fifo 2\n");
                fd2 = apertura_fifo_scrittura(myfifo2);
                scrittura_fifo_caratteri(fd2, message_send[i].seconda_parte);
                printf("Chiusura fifo 2\n");
                close(fd2);

                //Inserisco 1/4 nell'array file1/
                riempo_array_pari(file3, caratteri/4, open_file);
                strcpy(message_send[i].terza_parte, file3);
                printf("Message_send terza parte: %s\n", message_send[i].terza_parte);


                // /Inserisco 1/4 nell'array file1
                riempo_array_pari(file4, caratteri/4, open_file);
                strcpy(message_send[i].quarta_parte, file4);
                printf("Message_send quarta parte: %s\n", message_send[i].quarta_parte);


                printf("Primo semaforo\n");
                //inserisco il pid nella struttura
                printf("message_send.pid: %d\n", message_send[i].pid);
                //Stampo array per verifica
                //1/4 file 1
                stampo_array_divisione_file(file1, caratteri/4);
                stampo_array_divisione_file(file2, caratteri/4);
                stampo_array_divisione_file(file3, caratteri/4);
                stampo_array_divisione_file(file4, caratteri/4);
            }
                // File contenente un numero non multiplo di 4
            else {

                printf("CARATTERI DISPARI\n");

                char file1[caratteri/4+1];
                char file2[caratteri/4+1];
                char file3[caratteri/4+1];
                char file4[caratteri%4+1];

                //printf("resto: %ld\n", caratteri%4);
                //Inserisco 1/4 nell'array file1
                riempo_array_pari(file1, caratteri/4, open_file);
                strcpy(message_send[i].prima_parte, file1);
                printf("Message_send prima parte: %s\n", message_send[i].prima_parte);
                fd1 = apertura_fifo_scrittura(myfifo1);
                scrittura_fifo_caratteri(fd1, message_send[i].prima_parte);
                printf("Chiusura fifo 1\n");
                close(fd1);

                //Inserisco 1/4 nell'array file2
                riempo_array_pari(file2, caratteri/4, open_file);
                strcpy(message_send[i].seconda_parte, file2);
                printf("Message_send seconda parte: %s\n", message_send[i].seconda_parte);
                fd2 = apertura_fifo_scrittura(myfifo2);
                scrittura_fifo_caratteri(fd2, message_send[i].seconda_parte);
                printf("Chiusura fifo 2\n");
                close(fd2);

                //Inserisco 1/4 nell'array file1/
                riempo_array_pari(file3, caratteri/4, open_file);
                strcpy(message_send[i].terza_parte, file3);
                printf("Message_send terza parte: %s\n", message_send[i].terza_parte);

                // /Inserisco 1/4 nell'array file1
                riempo_array_pari(file4, caratteri/4+1, open_file);
                strcpy(message_send[i].quarta_parte, file4);
                printf("Message_send quarta parte: %s\n", message_send[i].quarta_parte);

                //inserico il pid nella struttura
                printf("message_send.pid: %d\n", message_send[i].pid);

                //Stampo array per verifica
                //1/4 file 1
                stampo_array_divisione_file(file1, caratteri/4+1);
                stampo_array_divisione_file(file2, caratteri/4+1);
                stampo_array_divisione_file(file3, caratteri/4+1);
                stampo_array_divisione_file(file4, caratteri/4+1);
            }
            semOp(semaforo_principale, (unsigned short) (i == 0)? count_sendme : i-1, 1);
            exit(0);
        }
        //Sono nel processo genitore
        if (wait(NULL) == -1)
            ErrExit("error wait");
    }
    //chiudo la cartella
    close_directory(dp);
    //detach from shared memory
    free_shared_memory(str);
    //destroy the shared memory
    remove_shared_memory(shmid);
    //remove message queue
    msgctl(msgid, IPC_RMID,  NULL);
    //remove semaphore
    if (semctl(semaforo_di_supporto, 0 /*ignored*/, IPC_RMID, NULL) == -1)
        ErrExit("semctl IPC_RMID failed");

    return 0;
}