/// @file fifo.h
/// @brief Contiene la definizioni di variabili e
///         funzioni specifiche per la gestione delle FIFO.

#pragma once

void creaFifo(char *myFifo);
void rimozioneFifo(char *myFifo);
int apertura_fifo_scrittura(char *myFifo);
int apertura_fifo_lettura(char *myFifo);
void scrittura_fifo(int fd, int file_sendme[]);
void lettura_fifo(int fd, int numero[]);