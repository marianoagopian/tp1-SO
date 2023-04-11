#ifndef _MAIN_H_
#define _MAIN_H_

// Crear imports para sacar los warnings y probar.

#define MD5_LENGTH 32
#define FILES_PER_SLAVE 2
#define PipeStdIn 0
#define PipeStdOut 1

#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/types.h>
#include<sys/wait.h>
#include <string.h>

#endif