#ifndef _SLAVE_H_
#define _SLAVE_H_

// Crear imports para sacar los warnings y probar.

#define MD5_LENGTH 32
#define FILES_PER_SLAVE 2
#define READ 0
#define WRITE 1

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include "./lib.h"

int slaveProcess(int * appToSlave, int * slaveToApp);

#endif