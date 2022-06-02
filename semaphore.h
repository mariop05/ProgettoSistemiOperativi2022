/// @file semaphore.h
/// @brief Contiene la definizioni di variabili e funzioni
///         specifiche per la gestione dei SEMAFORI.

#pragma once

int creazione_semaforo(key_t key, int numero_semafori, int flag);
void remove_semaforo(int semid);
void semOp(int semid, unsigned short sem_num, short sem_op);
void printSemaphoreValue(int semid, int n_sem);
void getSemaphoreId(int semid, char string[]);
void semSetAll(int semid, short unsigned values[], char *err);
int semOpNoBlocc(int semid, unsigned short sem_num, short sem_op);
int semWaitNoBloc(int semid, int sem_num);
void semSetVal(int semid, int values, char *err);