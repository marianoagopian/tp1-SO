#include "./include/slave.h"

int slaveProcess(int * appToSlave, int * slaveToApp) {
  //De app to slave tenemos que sacar los files de la linea 6
  //De slave to app tenemos mandar la respuesta de la linea 24
  int subToSlave[2];
  char *params[] = {"/usr/bin/md5sum", files[filesNum], NULL};
  char output[MD5_LENGTH + 1] = {0};


  if(pipe(subToSlave) == 0) {
    if(fork() == 0) {
      close(0);
      close(subToSlave[PipeStdIn]);
      dup2(subToSlave[PipeStdOut], 1);
      close(subToSlave[PipeStdOut]);
      execvp("/usr/bin/md5sum", params);
    } else {
      wait(NULL);
      close(subToSlave[PipeStdOut]);
      dup2(subToSlave[PipeStdIn], 0);
      close(subToSlave[PipeStdIn]);
      read(0, output, MD5_LENGTH + 1);
      printf("Md5: %s\n", output);
    }
  }
}