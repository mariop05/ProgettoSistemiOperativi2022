/// @file defines.c
/// @brief Contiene l'implementazione delle funzioni
///         specifiche del progetto.

#include <stdio.h>
#include <unistd.h>
#include "defines.h"
#include "err_exit.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#define SIZE 256
#define FILE_SIZE 4096


void create_directory(char *pathname){
    int res = mkdir(pathname, S_IRUSR | S_IXUSR | S_IWUSR | O_CREAT);
    if(res == -1)
        printf("Esiste già la cartella\n");
}
DIR* open_directory(char *pathname){
    if((opendir(pathname)) == NULL){
        ErrExit("error open directory\n");
    }
    return opendir(pathname);
}
int length_pathname(char *pathname){
    int i;
    //Trovo quanto è lungo il percorso e aggiungo /
    for(i = 0; pathname[i] != '\0'; i++);
    pathname[i] = '/';
    return i;
}
void create_pathname_format(struct dirent *dentry, int i, char *pathname){
    int j, k;
    for(j = i+1, k = 0; dentry->d_name[k] != '\0'; k++, j++) {
        pathname[j] = dentry->d_name[k];
    }
    pathname[j] = '\0';
    //printf("pathname complete: %s\n", pathname);
}
int equal_start_file(char sendMe[], struct dirent *dentry, int flag, char *pathname){
    int p;
    int count = 0;
    struct stat buffer;
    //Verifico che i file iniziano con 'sendme_'
    for(p = 0; sendMe[p] != '\0'; p++){
        //printf("sendMe[%d]:%c     pathname[%d]: %c\n",  p, sendMe[p], p, filename[p]);
        if(sendMe[p] != (dentry->d_name[p])){
            flag = 1;
        }
    }
//    //se flag = 0 -> il file inizia con 'sendme'
    if(flag == 0) {
        if (stat(pathname, &buffer) == -1)
            ErrExit("error stat\n");
//        printf("Pathname: %s\n", pathname);
        if (buffer.st_size <= FILE_SIZE) {
            //printf("Filename: %s   Size file: %ld Byte\n", dentry->d_name, buffer.st_size);
            count++;
            return count;
        }
    }

}
//int print_file(int flag, char *pathname, struct dirent *dentry, char file_pathname_sendme[100][250]){
//    int count = 0;
//    int j = 0, k = 0;
//    struct stat buffer;
//    //se flag = 0 -> il file inizia con 'sendme'
//    if(flag == 0) {
//        if (stat(pathname, &buffer) == -1)
//            ErrExit("error stat\n");
////        printf("Pathname: %s\n", pathname);
//        if (buffer.st_size <= FILE_SIZE) {
//            for(j = 0; j < )
//            printf("Filename: %s   Size file: %ld Byte\n", dentry->d_name, buffer.st_size);
//            count++;
//            return count;
//        }
//    }
//}
int close_directory(DIR *dir){
    //chiudo la cartella
    if((closedir(dir)) == -1)
        ErrExit("error close directory");
}

void riempo_array_pari(char array[], int caratteri, int fd_file) {
    int q;
    char carattere;

    for (q = 0; q < caratteri; q++) {
        // read a char from the file
        ssize_t bR = read(fd_file, &carattere, sizeof(char));
        //printf("c-> [%c]\n", c);
        // write the read char on standard output
        if (bR == sizeof(char)) {
            array[q] = carattere;
        }
    }
}
void stampo_array_divisione_file(char array[], int caratteri){
    int q;
    for(q = 0; q < caratteri; q++){
        printf("%c", array[q]);
    }
    printf("\n");
}
int conta_caratteri(int open_file){
    off_t caratteri = lseek(open_file, -1, SEEK_END);
    //printf("caratteri: %ld\n", caratteri);
    if(lseek(open_file, 0, SEEK_SET) == -1)
        ErrExit("error lseek");
    return caratteri;
}

