#include "./includes/main.h"

// Creating struct for slave
// need one way for app to comunicate with slave
// and another for slave to comunicate with app

typedef struct slave {
  int appToSlave[2];
  int slaveToApp[2];
} slave;

int main(int argc, char * argv[]) {
  char * files[argc];

  int filesNum = 0;

  if(argc <= 1) {
    fprintf(stderr, "%s\n", "No files found");
    return -1;
  }

  for(int i = 1 ; i < argc ; i++) {
    files[filesNum++] = argv[i];
  }

  //Checking how many slaves we need
  int slavesQty = filesNum / FILES_PER_SLAVE + 1; //The + 1 is there to avoid problems with casting

  slave slaves[slavesQty];

  //Initializing pipes for slaves
  for(int i = 0 ; i < slavesQty ; i++) {
    pipe(slaves[i].appToSlave);
    pipe(slaves[i].slaveToApp);
  }

  //Creating slaves
  int currSlave;
  int currId = 1;
  for(currSlave = 0 ; currSlave < slavesQty && currId != 0 ; currSlave++) {
    currId = fork();
  }

  if (currId == 0){
    close(slaves[currSlave-1].appToSlave[1]);
    close(slaves[currSlave-1].slaveToApp[0]);

    slaveProcess(slaves[currSlave - 1].appToSlave, slaves[currSlave - 1].slaveToApp);
  }
  else{
      // exit?
      //close useless pipes
      for( int i = 0 ; i < slavesQty ; i++ ){
        close(slaves[i].appToSlave[0]); //READ == 0
        close(slaves[i].slaveToApp[1]);
      }
      int filesSent = 0, filesRead = 0;

      // porq convendria seguir agregando los files mas tarde en vez d ahora ?
      for( int i = 0 ; filesSent < slavesQty ; i++){
        write(slaves[i].appToSlave[1], &(files[filesSent]), sizeof(char *));
        filesSent++;
      }

      //si no termino d leer
      for( int i = 0; i < slavesQty && filesRead < filesNum ; i++){
        //shared mem para el hash => implementar :)
        
        //semaforo?

        // if there is any file missing
        if(filesSent < filesNum){
          write(slaves[i].appToSlave[1], &(files[filesSent]),sizeof(char *));
          filesSent++;
        }
      }
  }

  printf("messi chiquito\n");
}
