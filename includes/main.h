#ifndef _MAIN_H_
#define _MAIN_H_

// Crear imports para sacar los warnings y probar.
typedef struct shmemInfo {
	char * name;
	int fd;
	void * mmap_addr;
} shmemInfo;

typedef struct hashInfo {
	int pid;
	char hash[MD5_LENGTH + 1];
	char file_name[256];
	//int files_left;
} hashInfo;

#define MD5_LENGTH 32
#define FILES_PER_SLAVE 2
#define PipeStdIn 0
#define PipeStdOut 1
#define _POSIX_SOURCE

#include <signal.h>
#include "slave.h"

#endif