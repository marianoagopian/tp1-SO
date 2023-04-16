#include "./includes/main.h"
#define SHNAME "md5"
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
    fprintf(stderr, "%s\n", "No files found");
    exit(1);
  }

  for(int i = 1 ; i < argc ; i++) {
    if(isFile(argv[i])) {
      files[filesNum++] = argv[i];
    }
  }

  fd_set fdRead, fdBackupRead;

  FILE * file;

  file = fopen("respuesta.txt", "w");

  FD_ZERO(&fdRead); //According to library you need to use this function before doing something with this library
  //Checking how many slaves we need
  int slavesQty = SLAVES_QTY(filesNum / FILES_PER_SLAVE + 1); //The + 1 is there to avoid problems with casting

  slave slaves[slavesQty];

  //Initializing pipes for slaves
  for(int i = 0 ; i < slavesQty ; i++) {
    pipe(slaves[i].appToSlave);
    pipe(slaves[i].slaveToApp);

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
    exit(1);
  }

  if(createSem(&semClose) == SEM_FAILED){
    unlinkShMem(shmem.name);
    unlinkSem(&semRead);
    exit(1);
  }

  sem_post(semClose.address);

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

  if (currId == 0) { // cerrar los demas esclavos?
    close(slaves[currSlave-1].appToSlave[WRITE]);
    close(slaves[currSlave-1].slaveToApp[READ]);

    slaveProcess(slaves[currSlave - 1].appToSlave, slaves[currSlave - 1].slaveToApp);
  } else {
      // exit?
      //  close useless pipes
      for( int i = 0 ; i < slavesQty ; i++ ){
        close(slaves[i].appToSlave[READ]);
        close(slaves[i].slaveToApp[WRITE]);
      }

      char currentHash[MD5_LENGTH + 1] = {0};
      int filesSent = 0, filesRead = 0;
      hashInfo bufToSend;

      // porq convendria seguir agregando los files mas tarde en vez d ahora ?
      for(int i = 0 ; filesSent < slavesQty ; i++){
        write(slaves[i].appToSlave[WRITE], &(files[filesSent]), sizeof(char *));  //hacer chequeo de errores en lib.c
        slaves[i].fileName = files[filesSent++];               
      }

      while(filesRead < filesNum) {
        // Check if a pipe is ready to be read
        // In case it's not select removes it from fdSet
        if(select(FD_SETSIZE, &fdRead, NULL, NULL, NULL) == -1) {
          perror("An error ocurred");  //ver todos los perror y exit
          exit(1);
        }

        //si no termino d leer
        for(int i = 0 ; i < slavesQty && filesRead < filesNum ; i++) {

          if(FD_ISSET(slaves[i].slaveToApp[READ], &fdRead)) { //This if is to check if the pipe slaves[i].slaveToApp has something in it
            if(read(slaves[i].slaveToApp[READ], currentHash, sizeof(char)*MD5_LENGTH) == -1) {
              perror("An error ocurred reading pipe");
              exit(1);
            }

            //Completing hashData
            bufToSend.pid = slaves[i].pid;
            strcpy(bufToSend.hash, currentHash);
            strcpy(bufToSend.file_name, slaves[i].fileName);
            // opcion alternativa strcat

            writeToShMem(shmem.fd, &bufToSend, sizeof(bufToSend),filesRead);
          
            sem_post(semRead.address);

            filesRead++;

            //Write in file
            fprintf(file, "File name: %s, MD5: %s, Slave id: %d\n", slaves[i].fileName, currentHash, slaves[i].pid);
            
            // if there is any file missing
            if(filesSent < filesNum) {
              write(slaves[i].appToSlave[WRITE], &(files[filesSent]), sizeof(char *));
              slaves[i].fileName = files[filesSent];
              filesSent++;
            }
          }
        }

        // This is because select (line 74) clears all file descriptors that are not ready to be read
        fdRead = fdBackupRead;
      }

      //  Free slaves
      for(int i = 0; i < slavesQty; i++){
        close(slaves[i].appToSlave[WRITE]);
        close(slaves[i].slaveToApp[READ]);

        kill(slaves[i].pid, SIGKILL);
      }

      closeShMem(&shmem);
      closeSem(&semRead);

      sem_wait(semClose.address);

      closeSem(&semClose);
  }
 
  fclose(file);
  return 0;
}
