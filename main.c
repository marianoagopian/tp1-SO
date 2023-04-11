#include "./includes/main.h"

int main(int argc, char * argv[]) {
  int myPipes[2];
  char *params[] = {"/bin/ls", NULL, NULL};
  char buff[244] = {0};
  if(pipe(myPipes) == 0) {
    if(fork() == 0) {
      close(0);
      close(myPipes[PipeStdIn]);
      dup2(myPipes[PipeStdOut], 1);
      close(myPipes[PipeStdOut]);
      execvp("/bin/ls", params);
    } else {
      wait(NULL);
      close(myPipes[PipeStdOut]);
      dup2(myPipes[PipeStdIn], 0);
      close(myPipes[PipeStdIn]);
      read(0, buff, 244);
      printf("Parent receives message: %s", buff);
    }
  }
}
