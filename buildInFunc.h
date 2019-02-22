#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "Shell.h"
#include "parse.h"
using namespace std;
/*check whether the variable name contain character which is not number, letter or underscores*/
bool isValidName(string s);

/*check whether the string is actually an integer */
bool isNum(string str);

/*inc command: increase the variable by 1. If it is not set or is not integer, set it as 1*/
void incVar(vector<string> command, map<string, string> & varMap);

/*set command: add variable to varMap for other command use. First parse set commandline as 2 or 3 argument and insert to map*/
void setVariable(vector<string> & command, string commandLine, map<string, string> & varMap);

/*export command: search variable map for value and set environment*/
void addEnv(vector<string> command, map<string, string> & varMap);

/*cd command: change the directory*/
void changeDirectory(vector<string> & command);
