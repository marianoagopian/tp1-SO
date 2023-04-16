#include "./includes/slave.h"

int slaveProcess(int * appToSlave, int * slaveToApp) {
  int subToSlave[2];
  char *params[] = {"/usr/bin/md5sum", NULL, NULL};
  char output[MD5_LENGTH + 1] = {0};
  char *fileName;

  while(1){
    read(appToSlave[0], &fileName, sizeof(char *));
    if(pipe(subToSlave) == 0) {
      if(fork() == 0) {
        close(READ);
        close(subToSlave[READ]);
        dup2(subToSlave[WRITE], 1);
        close(subToSlave[WRITE]);
        params[1] = fileName;
        execvp("/usr/bin/md5sum", params);
      } else {
        close(subToSlave[WRITE]);
        dup2(subToSlave[READ], 0);
        close(subToSlave[READ]);
        wait(NULL);
        read(READ, output, MD5_LENGTH);
        write(slaveToApp[WRITE], output, MD5_LENGTH*sizeof(char));
      }
    }

  }
  return 0;
}