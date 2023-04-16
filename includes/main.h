#ifndef _MAIN_H_
#define _MAIN_H_

#define MD5_LENGTH 32
#define FILES_PER_SLAVE 2
#define _POSIX_SOURCE
#define S_READ_NAME "md5_read_sem"
#define S_CLOSE_NAME "md5_close_sem"
#define SLEEP_TIME 2
#define SHMEM_SIZE 16384 // 4k

#include <signal.h>
#include "slave.h"
#include "./lib.h"

// Crear imports para sacar los warnings y probar.
typedef struct hashInfo {
	int pid;
	char hash[MD5_LENGTH + 1];
	char file_name[256];
	//int files_left;
} hashInfo;

#endif