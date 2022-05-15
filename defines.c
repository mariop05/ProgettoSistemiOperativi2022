/// @file defines.c
/// @brief Contiene l'implementazione delle funzioni
///         specifiche del progetto.

#include <stdio.h>
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

int equal_start_file(char sendMe[], struct dirent *dentry){
    int p;
    int flag = 0;
    //Verifico che i file iniziano con 'sendme_'
    for(p = 0; sendMe[p] != '\0'; p++){
        //printf("sendMe[%d]:%c     pathname[%d]: %c\n",  p, sendMe[p], p, filename[p]);
        if(sendMe[p] != (dentry->d_name[p])){
            flag = 1;
        }
    }
    return flag;

}
void create_pathname_format(struct dirent *dentry, int i, char *pathname){
    int j, k;
    for(j = i+1, k = 0; dentry->d_name[k] != '\0'; k++, j++) {
        pathname[j] = dentry->d_name[k];
    }
    pathname[j] = '\0';
    //printf("pathname complete: %s\n", pathname);
}
int print_file(int flag, struct stat buffer, char *pathname, struct dirent *dentry){
    int count = 0;
    //se flag = 0 -> il file inizia con 'sendme'
    if(flag == 0) {
        if (stat(pathname, &buffer) == -1)
            ErrExit("error stat\n");
//        printf("Pathname: %s\n", pathname);
        if (buffer.st_size <= FILE_SIZE) {
//          printf("file massimi sendme: %s\n", pathname);
            printf("Filename: %s   Size file: %ld Byte\n", dentry->d_name, buffer.st_size);
            return count++;
        }
    }
}
int close_directory(DIR *dir){
    //chiudo la cartella
    if((closedir(dir)) == -1)
        ErrExit("error close directory");
}

