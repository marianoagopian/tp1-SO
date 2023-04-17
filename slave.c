// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "./includes/slave.h"

int slaveProcess(int * appToSlave, int * slaveToApp) {
  int subToSlave[2];
  char *params[] = {"md5sum", NULL, NULL}; //removed /usr/bin/md5sum because of vps' suggestion
  char output[MD5_LENGTH + 1] = {0};
  char *fileName;

  while(1){
    read(appToSlave[0], &fileName, sizeof(char *));
    createPipe(subToSlave);
    if(createSlave() == 0) {
      closePipe(READ);
      closePipe(subToSlave[READ]);
      duplicateFD(subToSlave[WRITE], 1);
      closePipe(subToSlave[WRITE]);
      params[1] = fileName;
      execvp("md5sum", params);
    } else {
      closePipe(subToSlave[WRITE]);
      duplicateFD(subToSlave[READ], 0);
      closePipe(subToSlave[READ]);
      wait(NULL);
      read(READ, output, MD5_LENGTH);
      write(slaveToApp[WRITE], output, MD5_LENGTH*sizeof(char));
    }
  }
  return 0;
}