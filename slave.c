#include "./includes/slave.h"



int slaveProcess(int * appToSlave, int * slaveToApp) {
  //De app to slave tenemos que sacar los files de la linea 6
  //De slave to app tenemos mandar la respuesta de la linea 24
  int subToSlave[2];
  char *params[] = {"/usr/bin/md5sum", NULL, NULL};
  char output[MD5_LENGTH + 1] = {0};
  char *fileName;

  while(1){
    read(appToSlave[0], &fileName, sizeof(char *));
    if(pipe(subToSlave) == 0) {
      if(fork() == 0) {
        close(0);
        close(subToSlave[PipeStdIn]);
        dup2(subToSlave[PipeStdOut], 1);
        close(subToSlave[PipeStdOut]);
        params[1] = fileName;
        execvp("/usr/bin/md5sum", params);
      } else {
        close(subToSlave[PipeStdOut]);
        dup2(subToSlave[PipeStdIn], 0);
        close(subToSlave[PipeStdIn]);
        wait(NULL);
        read(0, output, MD5_LENGTH);
        write(slaveToApp[1], output, MD5_LENGTH*sizeof(char));
      }
    }

  }
  return 0;
}