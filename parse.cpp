#include "parse.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;
void parseEnviron(vector<string> & myvector, string env) {
  size_t start = 0;
  size_t found = env.find(":");
  while (found != string::npos) {
    myvector.push_back(env.substr(start, found - start));
    start = found + 1;
    found = env.find(":", start);
  }
  myvector.push_back(env.substr(start, string::npos));  //push last path
}

void parseCommand(vector<string> & cmdvector, string & cmd) {
  string arg;  //temporarily store each argument
  getline(cin, cmd);
  //if command encounter eof, it will exit
  if (std::cin.eof()) {
    exit(EXIT_SUCCESS);
  }
  size_t start = 0;
  size_t found = cmd.find(" ", start);
  while (found != string::npos) {
    if (cmd[found - 1] == '\\') {  //if it has \ before " ", it will not split the command
      arg = arg + cmd.substr(start, found - start - 1) + " ";
    }
    else {
      arg = arg + cmd.substr(start, found - start);
      if (!arg.empty()) {  //if whitespace is followed by whitespace, it will neglect the white space
        cmdvector.push_back(arg);
      }
      arg.clear();  //clear arg and begin to find next whitespace
    }
    start = found + 1;
    found = cmd.find(" ", start);
  }
  //push the last argument into the command vector
  arg = arg + cmd.substr(start);
  if (!arg.empty()) {
    cmdvector.push_back(arg);
  }
}
//for tesing parsing function
void printVector(vector<string> & myvector) {
  cout << "the vector is" << endl;
  for (size_t i = 0; i < myvector.size(); i++) {
    cout << myvector[i] << "**";
  }
  cout << endl;
}
//for set command
void parseBuildInCommand(vector<string> & command, string & commandLine) {
  size_t commandSize = command.size();  //record previous size
  command.clear();
  size_t start = 0;
  size_t found = commandLine.find(" ");  //first space after "set"
  command.push_back(commandLine.substr(start, found - start));
  //command[0] is now "set"
  start = found + 1;
  found = commandLine.find(" ", start);
  command.push_back(commandLine.substr(start, found - start));
  //command[1] is now the variable name
  if (commandSize == 2) {  //variable's value is empty
    command.push_back("");
  }
  else {  //command[2] is the value
    command.push_back(commandLine.substr(found + 1, string::npos));
  }
}
/*One argument can be a variable followed by some non-number and non-letter or some other variables. This function is to parse one argument by tracking the invalid char and $*/
string parseOneArgument(string & arg, map<string, string> & varMap) {
  size_t start = 0;
  size_t found = arg.find("$");
  string argument;
  if (found == string::npos) {  //do not contain $, return original argument
    return arg;
  }
  if (found != start) {  //still some character before $, we remain them
    argument.append(arg.substr(start, found - start));
  }
  while (found != string::npos) {
    start = found + 1;
    found = arg.find("$", start);
    //for the string between two $ (passible with some invalid char), we call parseOnevariable function to evaluate it
    argument.append(parseOneVariable(arg.substr(start, found - start), varMap));
  }
  //recursively call itself to evaluate every $ because it may need evaluate many times to get the final result
  argument = parseOneArgument(argument, varMap);
  return argument;
}
//evaluate a variable
string parseOneVariable(const string & var, map<string, string> & varMap) {
  string variable;
  size_t curr = 0;
  size_t end = var.size();
  map<string, string>::iterator it;
  while (curr != end) {  //record the first occurence position of invalid character
    if (!isdigit(var[curr]) && !isalpha(var[curr]) && var[curr] != 95) {
      break;
    }
    curr++;
  }
  it = varMap.find(var.substr(0, curr));
  if (it != varMap.end()) {
    variable.append(it->second);  //replace the key with value
  }
  //if we do not find it, we evaluate it as ""
  else {
    variable.append("");
  }
  variable.append(var.substr(curr));  //remain all the thing after variable
  return variable;
}

//pipe
vector<string> parsePipe(string commandLine) {
  vector<string> pipeCommand;
  size_t start = 0;
  size_t found = commandLine.find("|");
  while (found != string::npos) {
    pipeCommand.push_back(commandLine.substr(start, found - start));
    start = found + 1;
    found = commandLine.find("|", start);
  }
  pipeCommand.push_back(commandLine.substr(start, string::npos));
  return pipeCommand;
}
