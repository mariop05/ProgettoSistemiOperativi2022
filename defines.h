/// @file defines.h
/// @brief Contiene la definizioni di variabili
///         e funzioni specifiche del progetto.

#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#define SIZE 256
#pragma once

void create_directory(char *pathname);
DIR* open_directory(char *pathname);
int length_pathname(char *pathname);
void create_pathname_format(struct dirent *dentry, int i, char *pathname);
int equal_start_file(char sendMe[], struct dirent *dentry, int flag, char *pathname);
int print_file(int flag, char *pathname, struct dirent *dentry, char file_pathname_sendme[100][250]);
int close_directory(DIR *dir);
void sigHandlerINT(int sig);
void sigHandlerUSR1(int sig);
void riempo_array_pari(char array[], int caratteri, int fd_file);
void stampo_array_divisione_file(char array[], int caratteri);
int conta_caratteri(int open_file);
char stampa_username();