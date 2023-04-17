#include "./includes/main.h"

#define SLAVES_QTY(x) ((x) > 10 ? 10 : (x))

// Creating struct for slave
// need one way for app to comunicate with slave
// and another for slave to comunicate with app
typedef struct slave {
  int appToSlave[2];
  int slaveToApp[2];

  char * fileName;
  int pid;
} slave;


int main(int argc, char * argv[]) {
  char * files[argc];

  int filesNum = 0;

  if(argc <= 1) {
    perror("No files found");
    exit(ERROR_NO_FILES);
  }

  for(int i = 1 ; i < argc ; i++) {
    if(isFile(argv[i])) {
      files[filesNum++] = argv[i];
    }
  }

  fd_set fdRead, fdBackupRead;

  FILE * file = openFile("respuesta.txt", "w");

  FD_ZERO(&fdRead); //According to library you need to use this function before doing something with this library
  //Checking how many slaves we need
  int slavesQty = SLAVES_QTY(filesNum / FILES_PER_SLAVE + 1); //The + 1 is there to avoid problems with casting

  slave slaves[slavesQty];

  //Initializing pipes for slaves
  for(int i = 0 ; i < slavesQty ; i++) {
    createPipe(slaves[i].appToSlave);
    createPipe(slaves[i].slaveToApp);

    FD_SET(slaves[i].slaveToApp[READ], &fdRead);
  }

  fdBackupRead = fdRead;

  shmemInfo shmem;
  shmem.name = SHNAME;

  semInfo semRead, semClose;
  semRead.name = S_READ_NAME;
  semClose.name = S_CLOSE_NAME;

  createShMem(&shmem);

  if(createSem(&semRead) == SEM_FAILED){
    unlinkShMem(shmem.name);
    perror("An error occurred while creating the semaphore");
    exit(ERROR_CREATING_SEM);
  }

  if(createSem(&semClose) == SEM_FAILED){
    unlinkShMem(shmem.name);
    unlinkSem(&semRead);
    perror("An error occurred while creating the semaphore");
    exit(ERROR_CREATING_SEM);
  }

  sem_post(semClose.address);

  setvbuf(stdout, NULL, _IONBF, 0);

  printf("%s\n", SHNAME);
  printf("%s\n", S_READ_NAME);
  printf("%s\n", S_CLOSE_NAME);

  sleep(SLEEP_TIME);

  //Creating slaves
  int currSlave;
  int currId = 1;
  for(currSlave = 0 ; currSlave < slavesQty && currId != 0 ; currSlave++) {
    currId = fork();
    slaves[currSlave].pid = currId;
  }

  if (currId == 0) {
    closePipe(slaves[currSlave-1].appToSlave[WRITE]);
    closePipe(slaves[currSlave-1].slaveToApp[READ]);

    slaveProcess(slaves[currSlave - 1].appToSlave, slaves[currSlave - 1].slaveToApp);
  } else {
      // close useless pipes
      for( int i = 0 ; i < slavesQty ; i++ ){
        closePipe(slaves[i].appToSlave[READ]);
        closePipe(slaves[i].slaveToApp[WRITE]);
      }

      char currentHash[MD5_LENGTH + 1] = {0};
      int filesSent = 0, filesRead = 0;
      hashInfo bufToSend;
      memset(&bufToSend, 0, sizeof(hashInfo));

      for(int i = 0 ; filesSent < slavesQty ; i++){
        write(slaves[i].appToSlave[WRITE], &(files[filesSent]), sizeof(char *));  //hacer chequeo de errores en lib.c
        slaves[i].fileName = files[filesSent++];
      }

      while(filesRead < filesNum) {
        // Check if a pipe is ready to be read
        // In case it's not select removes it from fdSet
        if(select(FD_SETSIZE, &fdRead, NULL, NULL, NULL) == -1) {
          perror("An error ocurred related with file descriptors"); 
          exit(ERROR_SELECT);
        }

        for(int i = 0 ; i < slavesQty && filesRead < filesNum ; i++) {

          if(FD_ISSET(slaves[i].slaveToApp[READ], &fdRead)) { //This if is to check if the pipe slaves[i].slaveToApp has something in it
            if(read(slaves[i].slaveToApp[READ], currentHash, sizeof(char)*MD5_LENGTH) == -1) {
              perror("An error ocurred reading pipe");
              exit(ERROR_READING_SLAVE_PIPE);
            }

            // Creating object with info to send to Share Memory
            bufToSend.pid = slaves[i].pid;
            strcpy(bufToSend.hash, currentHash);
            strcpy(bufToSend.fileName, slaves[i].fileName);

            bufToSend.filesLeft = filesNum - filesRead;
            writeToShMem(shmem.fd, &bufToSend, sizeof(hashInfo), filesRead);
          
            sem_post(semRead.address);

            filesRead++;

            //Write in file
            fprintf(file, "Slave id: %d | MD5: %s | File Name: %s\n", bufToSend.pid, bufToSend.hash, bufToSend.fileName);
            
            // if there is any file missing
            if(filesSent < filesNum) {
              write(slaves[i].appToSlave[WRITE], &(files[filesSent]), sizeof(char *));
              slaves[i].fileName = files[filesSent];
              filesSent++;
            }
          }
        }

        // This is because select (line 118) clears all file descriptors that are not ready to be read
        fdRead = fdBackupRead;
      }

      //  Free slaves
      for(int i = 0; i < slavesQty; i++){
        closePipe(slaves[i].appToSlave[WRITE]);
        closePipe(slaves[i].slaveToApp[READ]);

        kill(slaves[i].pid, SIGKILL);
      }

      closeShMem(&shmem);
      closeSem(&semRead);

      sem_wait(semClose.address);

      closeSem(&semClose);

      
      closeFile(file);
      unlinkShMem(shmem.name);
      unlinkSem(&semClose);
      unlinkSem(&semRead);
  }
  return 0;
}
