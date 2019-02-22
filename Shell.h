#ifndef SHELL_H
#define SHELL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

#include "parse.h"
using namespace std;
class Shell
{
 private:
  string commandLine;
  vector<string> command;  //array of parsed command
  bool Exit, nextCommand;  //status
  static map<string, string> varMap;

 public:
  /*Constructor: initialize Shell*/
  Shell();
  /*check command type -- including empty command, set, export, inc, cd, exit and command without slash. For each case, we set its Exit or nextCommand to indicate whether it will continue or break and will not fork a new process(except command without slash).*/
  void checkCommandType();

  /*Helper function which recursively call itself to enter each sub directory until and compare the filename with command name. If it is same, get its full path name and return true, else return false*/
  bool findFullPath_helper(string & command, string dirname);

  /*Find full path name of the command. Parse ECE551PATH 's value and store in a vector. For each path, enter each directory to search. */
  bool findFullPathName(string & command);

  /*after checking "set" "inc" "export" command, evaluate the variable -- First parse the variable ($) and then search the variable Map to find the variable's value and replace it*/
  void replaceVariable();

  /*get private member Exit*/
  bool isExit();

  /*get private member nextCommand*/
  bool isNextCommand();

  /*run command: convert vector<string> into char *[] to pass to execve() system call and then run the command.*/
  void runCommand();

  void checkRedirection();
  void redirection(string filename, char choice);
  void outputRedirection(string filename);
  void errorRedirection(string filename);
  void checkPipe();
  void setPipe();
};
#endif
