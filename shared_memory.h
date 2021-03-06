/// @file shared_memory.h
/// @brief Contiene la definizioni di variabili e funzioni
///         specifiche per la gestione della MEMORIA CONDIVISA.

#pragma once
#ifndef _SHARED_MEMORY_HH
#define _SHARED_MEMORY_HH

#include <stdlib.h>

int generateUniqueKey();
int alloc_shared_memory(key_t key, size_t size, int shmflg);
void *get_shared_memory(int shmid, int shmflg);
void free_shared_memory(char *str);
void remove_shared_memory(int shmid);
int generate_unique_key_message_queue();
int create_message_queue(key_t key);
#endif