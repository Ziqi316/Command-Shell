#include "buildInFunc.h"

#include <assert.h>
#include <limits.h>
#include <string.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

using namespace std;
bool isValidName(string s) {
  for (size_t j = 0; j < s.size(); j++) {
    if (!isdigit(s[j]) && !isalpha(s[j]) && s[j] != 95) {  //is not letter, number and underscores
      return false;
    }
  }
  return true;
}

bool isNum(string str) {
  //check each character of the string is number
  if (str.length() == 1 && !isdigit(str[0])) {  //if only one character, must be number
    return false;
  }
  for (size_t i = 1; i < str.length(); i++) {
    if (!isdigit(str[i])) {  //if not digit
      return false;
    }
    else {  //if not digit
            //check whether first character is negative sign or digit
      if (!isdigit(str[0]) && str[0] != '-') {
        return false;
      }
    }
  }
  return true;
}
//inc command
void incVar(vector<string> command, map<string, string> & varMap) {
  if (command.size() != 2) {
    cerr << "Should have one argument!" << endl;
    return;
  }
  if (!isValidName(command[1])) {  //variable name should be valid
    cout << "variables should be numbers, letters and underscores!" << endl;
    return;
  }
  map<string, string>::iterator it = varMap.find(command[1]);
  if (it != varMap.end()) {
    if (isNum(it->second)) {
      long num = atol((it->second).c_str());  //is integer: convert to long int
      if (num == LONG_MAX) {
        it->second = "1";
      }
      else {
        num = num + 1;
        stringstream ss;
        ss << num;  //using stringstream to convert int to string
        it->second = ss.str();
      }
    }
    else {  //find but not integer: initialize as 1
      it->second = "1";
    }
  }
  else {  //did not find, insert it
    varMap.insert(pair<string, string>(command[1], "1"));
  }
  return;
}
//set command: add to varMap
void setVariable(vector<string> & command, string commandLine, map<string, string> & varMap) {
  if (command.size() == 1) {  //no variable
    cerr << "Should have one argument!" << endl;
    return;
  }
  //parse it again and command will only have "set", variable, name three elements
  parseBuildInCommand(command, commandLine);
  if (!isValidName(command[1])) {  //variable name should be valid
    cout << "variables should be numbers, letters and underscores!" << endl;
    return;
  }
  std::map<string, string>::iterator it = varMap.find(command[1]);
  if (it != varMap.end()) {  //find: change its value by first erase and then insert
    varMap.erase(it);
  }
  //add to map
  varMap.insert(pair<string, string>(command[1], command[2]));
  return;
}
//export command: add to environment
void addEnv(vector<string> command, map<string, string> & varMap) {
  if (command.size() != 2) {
    cerr << "Usage: should have one argument!" << endl;
    return;
  }
  if (!isValidName(command[1])) {  //variable name should be valid
    cout << "variables should be numbers, letters and underscores!" << endl;
    return;
  }
  map<string, string>::iterator it = varMap.find(command[1]);
  if (it != varMap.end()) {  //find: set environment
    setenv(command[1].c_str(), it->second.c_str(), 1);
    return;
  }
  //cerr << "cannot add enc" << endl;
  //if do not find, no notice for user as a real bash
}
//cd command
void changeDirectory(vector<string> & command) {
  if (command.size() > 2) {
    cerr << "cd: too many arguments!" << endl;
    return;
  }
  else {
    if (command.size() == 1) {  // only cd: enter HOME
      command.push_back(getenv("HOME"));
    }
    int ret = chdir(command[1].c_str());
    if (ret) {  //cd failure
      perror("cd");
    }
  }
}
