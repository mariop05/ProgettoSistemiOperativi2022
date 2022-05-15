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
int equal_start_file(char sendMe[], struct dirent *dentry);
void create_pathname_format(struct dirent *dentry, int i, char *pathname);
int print_file(int flag, struct stat buffer, char *pathname, struct dirent *dentry);
int close_directory(DIR *dir);
void sigHandlerINT(int sig);
void sigHandlerUSR1(int sig);