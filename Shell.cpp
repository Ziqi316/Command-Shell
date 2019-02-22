#include "Shell.h"

#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
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
#include <sstream>
#include <stdexcept>
#include <vector>

#include "buildInFunc.h"
map<string, string> Shell::varMap;
//initialize the shell
Shell::Shell() {
  Exit = false;
  nextCommand = false;
  parseCommand(command, commandLine);  //initialize command from stdin and parse command
}
void Shell::checkCommandType() {
  //only enter \n and whitespace
  if (command.empty()) {
    nextCommand = true;
    return;
  }
  replaceVariable();
  //check "set"
  if (command[0] == "set") {
    nextCommand = true;
    setVariable(command, commandLine, varMap);
    replaceVariable();  //we parse it again so need replace again. Note that variable name still cannot have $ because we deal with this situation in setVariable() function.
    return;
  }
  //check "export"
  if (command[0] == "export") {
    nextCommand = true;
    addEnv(command, varMap);
    return;
  }
  //check "inc"
  if (command[0] == "inc") {
    nextCommand = true;
    incVar(command, varMap);
    return;
  }
  //After we check 3 build in function,we can evaluate the variable

  //check "cd"
  if (command[0] == "cd") {
    nextCommand = true;
    changeDirectory(command);
    return;
  }
  //check "exit"
  if (command[0] == "exit") {
    Exit = true;
    return;
  }
  //check command without slash
  for (size_t i = 0; i < command[0].size(); i++) {
    if (command[0][i] == '/') {
      return;
    }
  }
  //if it do not have slash--we need to find its full path
  if (!findFullPathName(command[0])) {
    cout << "Command " << command[0] << " not found" << endl;
    nextCommand = true;
  }
}
bool Shell::findFullPath_helper(string & command, string dirname) {
  DIR * dir;
  struct dirent * dirp;
  //open directory
  if ((dir = opendir(dirname.c_str())) == NULL) {
    cerr << "Cannot open the directory!" << endl;
    closedir(dir);
    nextCommand = true;
    return false;
  }
  //read every file in directory dir
  while ((dirp = readdir(dir)) != NULL) {
    string filename(dirp->d_name);
    if (filename == "." || filename == "..") {
      continue;
    }
    //if it is a sub directory, we recursively call the function to enter to
    else if (dirp->d_type == DT_DIR) {
      std::string subdir = dirname + "/" + filename;
      if (findFullPath_helper(command, subdir)) {
        closedir(dir);
        return true;
      }
    }
    //if it is a file, compare file name with command name
    else if (dirp->d_type == DT_REG) {
      if (filename == command) {  //find -> get full path name
        command = dirname + "/" + filename;
        closedir(dir);
        return true;
      }
    }
  }
  //did not find command
  closedir(dir);
  delete dirp;
  return false;
}

bool Shell::findFullPathName(string & command) {
  vector<string> path;
  string ece551path_str(getenv("ECE551PATH"));
  //parse the path list (:) and enter each path
  parseEnviron(path, ece551path_str);
  for (size_t i = 0; i < path.size(); i++) {
    if (findFullPath_helper(command, path[i])) {
      return true;
    }
  }
  return false;
}

bool Shell::isExit() {
  return Exit;
}
bool Shell::isNextCommand() {
  return nextCommand;
}

void Shell::runCommand() {
  //convert vector<string> to char *[] to pass to execve() system call
  char ** argv = new char *[command.size() + 1];
  for (size_t i = 0; i < command.size(); i++) {
    argv[i] = new char[command[i].length() + 1];
    strcpy(argv[i], command[i].c_str());
  }
  argv[command.size()] = NULL;
  execve(command[0].c_str(), argv, environ);
  perror("execve");
  //delete char *[]
  for (size_t i = 0; i < command.size(); i++) {
    delete[] argv[i];
  }
  delete[] argv;
  //free vector to avoid memory leak when exiting failure
  vector<string>().swap(command);
  exit(EXIT_FAILURE);
}
void Shell::replaceVariable() {
  //check each argument and evaluate them
  for (size_t i = 0; i < command.size(); i++) {
    command[i] = parseOneArgument(command[i], varMap);
  }
}

void Shell::checkRedirection() {
  for (size_t i = 0; i < command.size(); i++) {  //if <is the last argument, we omit it
    if (command[i] == "<" || command[i] == ">" || command[i] == "2>") {  //  <(space)filename
      if (i == command.size() - 1) {  //if > < 2> is the last elements
        redirection("", command[i][0]);
      }
      redirection(command[i + 1], command[i][0]);
      std::vector<string>::iterator it = command.begin() + i;
      std::vector<string>::iterator it1 = command.begin() + i + 1;
      command.erase(it);   //will not passed as parameter to execve call
      command.erase(it1);  //erase two arguments
      i = i - 2;
    }
    else {  //<filename also works
      if (command[i][0] == '2' && command[i][1] == '>') {
        redirection(command[i].substr(2), '2');
      }
      else if (command[i][0] == '<' || command[i][0] == '>') {
        redirection(command[i].substr(1), command[i][0]);
      }
      else {
        continue;
      }
      std::vector<string>::iterator it = command.begin() + i;
      command.erase(it);
      i--;
    }
  }
}
void Shell::redirection(string filename, char choice) {
  if (filename.empty()) {  //only a < > or 2> without filename
    cerr << "syntax error near unexpected toekn 'newline'" << endl;
    vector<string>().swap(command);
    exit(EXIT_FAILURE);
  }
  char * file = new char[filename.length() + 1];
  strcpy(file, filename.c_str());
  int fd;
  if (choice == '<') {  //in input redirection, if the file does not exist, do not create it
    fd = open(file, O_RDWR, 0644);
  }
  else {
    fd = open(file, O_RDWR | O_CREAT, 0644);
  }
  if (fd == -1) {  //open error
    perror("<");
    vector<string>().swap(command);
    delete[] file;
    exit(EXIT_FAILURE);
  }
  int refd = 0;
  if (choice == '<')  //input redirection
    refd = dup2(fd, 0);
  else if (choice == '>')  //output redirection
    refd = dup2(fd, 1);
  else if (choice == '2')  //error redirection
    refd = dup2(fd, 2);
  if (refd == -1) {  //redirect error
    cout << "redirect standard in error" << endl;
    delete[] file;
    exit(EXIT_FAILURE);
  }
  close(fd);
  delete[] file;
}
/* Have not implemented
void Shell::checkPipe() {
  size_t i = 0;
  while (command[i] != "|") {
    cout << "checkPipe" << endl;
    vector<string> pipeCommand = parsePipe(commandLine);
    printVector(pipeCommand);
    for (vector<string>::iterator it = command.begin(); it != command.end(); ++it) {
      if (*it == "|") {
        break;
      }
      //i++;
    }
    if (i == command.size() - 1 || i == command.size()) {
      return;
    }
    int fd[2];
    int ret = pipe(fd);
    if (ret == -1) {  // pipe failure
      perror("pipe error:");
      exit(EXIT_FAILURE);
    }
    pid_t fpid = fork();
    if (fpid < 0) {
      perror("fork");
    }
    else if (fpid == 0) {
      if (dup2(fd[0], STDOUT_FILENO) < 0) {
        perror("fd[0]");
        exit(EXIT_FAILURE);
      }  //redirect fd[0] as output
      if (dup2(fd[1], STDIN_FILENO) < 0) {
        perror("fd[1]");
        exit(EXIT_FAILURE);

      }  //redirect fd[1] as input
      close(fd[0]);
      close(fd[1]);
      char ** argv = new char *[command.size() - i - 1];
      for (size_t j = 0; j < command.size() - i - 1; j++) {
        argv[j] = new char[command[j + i + 1].length() + 1];
        strcpy(argv[j], command[j + i + 1].c_str());
      }
      argv[command.size() - i - 2] = NULL;

      execve(argv[0], argv, environ);
      //execve(command[i + 1].c_str(), argv, environ);
      perror("execve");
    }
    else {
      pid_t wpid;
      int status = 0;
      while ((wpid = wait(&status)) > 0) {
        if (WIFEXITED(status)) {
          printf("Program exited with status %d\n", WEXITSTATUS(status));
        }
      }
    }
    vector<string> newvec(command);
    size_t command_size = command.size();
    command.clear();
    for (size_t j = 0; j < command_size - i - 1; j++) {
      command[j] = newvec[j];
    }

    for (size_t j = 0; j < command_size - i - 1; j++) {
      command.pop_back();
    }
  }
}
*/
