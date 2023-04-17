#include "./includes/lib.h"

// Files
FILE * openFile(char *fileName, char * mode) {
    FILE * toReturn;
    if((toReturn = fopen(fileName, mode)) == NULL) {
        perror("An error occurred while creating file");
        exit(ERROR_CREATING_FILE);
    }

    return toReturn;
} 

void closeFile(FILE * file){
    if(fclose(file) == EOF){
        perror("An error occurred while closing file");
        exit(ERROR_CLOSING_FILE);
    }
}

// Shared Memory

void openShMem(shmemInfo * data) {
    if((data->fd = shm_open(data->name, O_RDONLY, S_IRUSR)) == -1) {
        perror("An error occurred while creating shared memory");
        exit(ERROR_CREATING_SHMEM);
    }

    if((data->mmapAddress = mmap(NULL, SHMEM_SIZE, PROT_READ, MAP_SHARED, data->fd, 0)) == MAP_FAILED) {
        perror("An error occurred related with mmap shared memory");
        exit(ERROR_MMAP_SHMEM);
    }
}

void createShMem(shmemInfo * data) {
    if((data->fd = shm_open(data->name, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR)) == -1){
        perror("An error occurred while creating shared memory");
        exit(ERROR_CREATING_SHMEM);
    }
    if(ftruncate(data->fd, SHMEM_SIZE) == -1) {
        perror("An error occurred related with ftruncate shared memory");
        exit(ERROR_FTRUNCATE_SHMEM);
    }
    if((data->mmapAddress = mmap(NULL, SHMEM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, data->fd, 0)) == MAP_FAILED) {
        perror("An error occurred related with mmap shared memory");
        exit(ERROR_MMAP_SHMEM);
    }
}

void closeShMem(shmemInfo * data) {
    if(munmap(data->mmapAddress, SHMEM_SIZE) == -1) {
        perror("An error occurred related with munmap shared memory");
        exit(ERROR_MUNMAP_SHMEM);
    }

    closePipe(data->fd);
}

void unlinkShMem(char * name) {
    if(shm_unlink(name) == -1){
        perror("An error occurred while unlinking shared memory");
        exit(ERROR_UNLINKING_SHMEM);
    }
}

void writeToShMem(int fd, void * data, size_t length, int pos) {
    if(pwrite(fd, data, length, pos * length) == -1) {
        perror("An error occurred while writing pipe");
        exit(ERROR_WRITING_PIPE);
    }
}

//Semaphores

void * createSem(semInfo * data) {
    return (data->address = sem_open(data->name, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR, 0));
}

void openSem(semInfo * data){
    if((data->address = sem_open(data->name, O_RDONLY, S_IRUSR, 0)) == SEM_FAILED) {
        perror("An error occurred while opening semaphore");
        exit(ERROR_OPENING_SEM);
    }
}

void closeSem(semInfo * data) {
    if(sem_close(data->address) == -1) {
        perror("An error occurred while closing semaphore");
        exit(ERROR_CLOSING_SEM);
    }
}

void unlinkSem(semInfo * data) {
    if(sem_unlink(data->name) == -1) {
        perror("An error occurred while unlinking semaphore");
        exit(ERROR_UNLINKING_SEM);
    }
}

// Check file
int isFile(const char * filename) {
    struct stat buffer;
    stat(filename, &buffer);
    return S_ISREG(buffer.st_mode);
}

// File Descriptores
void createPipe(int fd[2]) {
    if(pipe(fd) == -1) {
        perror("An error occured while creating pipe");
        exit(ERROR_CREATING_PIPE);
    }
}

void closePipe(int fd) {
    if(close(fd) == -1) {
        perror("An error occurred while closing pipe");
        exit(ERROR_CLOSING_PIPE);
    }
}

void duplicateFD(int oldFd, int newFd) {
    if(dup2(oldFd, newFd) == -1) {
        perror("An error occurred while duplicating pipe");
        exit(ERROR_DUPLICATING_PIPE);
    }
}

// Slave
int createSlave() {
    int toReturn;
    if((toReturn = fork()) == -1) {
        perror("An error ocurred while creating slave");
        exit(ERROR_CREATING_SLAVE);
    }
    return toReturn;
}