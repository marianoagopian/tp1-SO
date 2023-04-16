#include "./includes/lib.h"

// Shared Memory

void openShMem(shmemInfo * data) {
    if((data->fd = shm_open(data->name, O_RDONLY, S_IRUSR)) == -1) {
        exit(1);
    }

    if((data->mmap_address = mmap(NULL, SHMEM_SIZE, PROT_READ, MAP_SHARED, data->fd, 0)) == MAP_FAILED) {
        exit(1);
    }
}

void createShMem(shmemInfo * data) {
    if((data->fd = shm_open(data->name, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR)) == -1){
        exit(1);
    }
    if(ftruncate(data->fd, SHMEM_SIZE) == -1) {
        exit(1);
    }
    if((data->mmap_address = mmap(NULL, SHMEM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, data->fd, 0)) == MAP_FAILED) {
        exit(1);
    }
}

void closeShMem(shmemInfo * data) {
    if(munmap(data->mmap_address, SHMEM_SIZE) == -1) {
        exit(1);
    }

    if(close(data->fd) == -1) {
        exit(1);
    }
}

void unlinkShMem(char * name) {
    if(shm_unlink(name) == -1){
        exit(1);
    }
}

void writeToShMem(int fd, void * data, size_t length, int pos) {
    if(pwrite(fd, data, length, pos * length) == -1) {
        exit(1);
    }
}

//Semaphores

void * createSem(semInfo * data) {
    return (data->address = sem_open(data->name, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR, 0));
}

void openSem(semInfo * data){
    if((data->address = sem_open(data->name, O_RDONLY, S_IRUSR, 0)) == SEM_FAILED) {
        exit(1);
    }
}

void closeSem(semInfo * data) {
    if(sem_close(data->address) == -1) {
        exit(1);
    }
}

void unlinkSem(semInfo * data) {
    if(sem_unlink(data->name) == -1) {
        exit(1);
    }
}

// Check file
int isFile(const char * filename) {
    struct stat buffer;
    stat(filename, &buffer);
    return S_ISREG(buffer.st_mode);
}