#ifndef _LIB_H_
#define _LIB_H_


// These three defines are for imports to work properly
#define _POSIX_SOURCE
#define _BSD_SOURCE
#define  _XOPEN_SOURCE 500

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

// These define avoid magic numbers
#define SHMEM_SIZE 16384 //4k
#define MAX_BUFFER 256
#define MD5_LENGTH 32
#define FILES_PER_SLAVE 2
#define READ 0
#define WRITE 1
#define SHNAME "md5"
#define S_READ_NAME "md5_read_sem"
#define S_CLOSE_NAME "md5_close_sem"
#define SLEEP_TIME 2

typedef struct semInfo {
	char * name;
	sem_t * address;
} semInfo;

typedef struct shmemInfo {
	char * name;
	int fd;
	void * mmapAddress;
} shmemInfo;

typedef struct hashInfo {
	int pid;
	char hash[MD5_LENGTH + 1];
	char fileName[MAX_BUFFER];
	int filesLeft;
} hashInfo;

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
