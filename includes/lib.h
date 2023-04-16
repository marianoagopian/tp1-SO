#ifndef _LIB_H_
#define _LIB_H_

#define _POSIX_SOURCE
#define _BSD_SOURCE
#define  _XOPEN_SOURCE 501

#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define SHMEM_SIZE 16384 //4k

typedef struct semInfo {
	char * name;
	sem_t * address;
} semInfo;

typedef struct shmemInfo {
	char * name;
	int fd;
	void * mmap_address;
} shmemInfo;


// Shared Memory
void openShMem(shmemInfo * data);
void createShMem(shmemInfo * data);
void closeShMem(shmemInfo * data);
void unlinkShMem(char * name);
void writeToShMem(int fd, void * data, size_t length, int pos);

//Semaphores
void * createSem(semInfo * data);
void openSem(semInfo * data);
void closeSem(semInfo * data);
void unlinkSem(semInfo * data);

// Check file
int isFile(const char * filename);

#endif
