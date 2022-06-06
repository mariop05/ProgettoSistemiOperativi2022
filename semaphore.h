/// @file semaphore.h
/// @brief Contiene la definizioni di variabili e funzioni
///         specifiche per la gestione dei SEMAFORI.

#pragma once

#include <stdlib.h>

int creazione_semaforo(key_t key, int numero_semafori, int flag);
void remove_semaforo(int semid);
void semOp(int semid, unsigned short sem_num, short sem_op);