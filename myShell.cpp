#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

#include "Shell.h"
extern char ** environ;
using namespace std;
//show current directory
void displayShell() {
  char * directory;
  directory = getcwd(NULL, 0);
  cout << "myShell:" << directory << "$ ";
  free(directory);
}

int main(void) {
  setenv("ECE551PATH", getenv("PATH"), 1);  //initialize ECE551PATH
  while (true) {
    displayShell();
    Shell myShell;
    myShell.checkCommandType();
    //if command is "exit"
    if (myShell.isExit()) {
      break;
    }
    //if command is built in -- do not need to fork new process
    if (myShell.isNextCommand()) {
      continue;
    }

    int status;
    pid_t wpid;
    pid_t fpid = fork();
    //fork error
    if (fpid < 0) {
      perror("fork");
      exit(EXIT_FAILURE);
    }

    if (fpid == 0) { /*child process*/
      myShell.checkRedirection();
      //myShell.checkPipe();
      myShell.runCommand();
    }

    else { /*parent process*/
      wpid = wait(&status);
      if (wpid == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
      }
      if (WIFEXITED(status)) {
        cout << "Program exited with status " << WEXITSTATUS(status) << endl;
      }
      if (WIFSIGNALED(status)) {
        cout << "Program was killed by signal " << WTERMSIG(status) << endl;
      }
    }
  }
  return (EXIT_SUCCESS);
}
