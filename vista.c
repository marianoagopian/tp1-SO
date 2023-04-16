#include "./includes/vista.h"

int main(int argc, char *argv[]) {
  char shmemName[MAX_BUFFER], semReadName[MAX_BUFFER], semCloseName[MAX_BUFFER];
  shmemInfo shmem;
  semInfo semRead, semClose;
  hashInfo hash;

  if(argc >= 4) {
    // If function is called from two diffrent terminals
    strncpy(shmemName, argv[1], MAX_BUFFER);
    strncpy(semReadName, argv[2], MAX_BUFFER);
    strncpy(semCloseName, argv[3], MAX_BUFFER);
  } else {
    // If function is called ./main <args> | ./vista
    fgets(shmemName, MAX_BUFFER, stdin);
    fgets(semReadName, MAX_BUFFER, stdin);
    fgets(semCloseName, MAX_BUFFER, stdin);

    shmemName[strlen(shmemName) - 1] = 0;
    semReadName[strlen(semReadName) - 1] = 0; 
    semCloseName[strlen(semCloseName) - 1] = 0;
  }
  

  shmem.name = shmemName;
  semRead.name = semReadName;
  semClose.name = semCloseName;

  openShMem(&shmem);
  openSem(&semRead);
  openSem(&semClose);

  sem_wait(semClose.address);

  //If this line isn't the program doesn't print all hashes
  setvbuf(stdout, NULL, _IONBF, 0);

  for(int i = 0, finished = 0 ; !finished ; i++) {
    sem_wait(semRead.address);

    if(pread(shmem.fd, &hash, sizeof(hashInfo), i * sizeof(hashInfo)) == -1) {
      sem_post(semClose.address);
      exit(1);
    }

    printf("File name: %s | MD5: %s | Slave id: %d\n", hash.fileName, hash.hash, hash.pid);

    if(hash.filesLeft <= 1) {
      finished = 1;
    }
  }

  sem_post(semClose.address);
  closeShMem(&shmem);
  closeSem(&semRead);
  closeSem(&semClose);

  return 0;
}
