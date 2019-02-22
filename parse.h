#ifndef PARSE_H
#define PARSE_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>
using namespace std;
/*test function: print the vector to see whether the parse is right*/
void printVector(vector<string> & myvector);

/*split the command line with whitespace*/
void parseCommand(vector<string> & myvector, string & str);

/*split environment list with :*/
void parseEnviron(vector<string> & myvector, string env);

/*split one argument with $*/
string parseOneArgument(string & arg, map<string, string> & varMap);

/*spit a variable with some invalid character and replace the variable.Check the string between two $ or between a $ and a whitespace. We only need to find the first invalid char and search the string before it in the map. If it finds the value, the value will replace it.*/
string parseOneVariable(const string & var, map<string, string> & varMap);

/*split the command with whitespace to 2 or 3 argument (for set command) */
void parseBuildInCommand(vector<string> & command, string & commandLine);

/*split command with | for pipe*/
vector<string> parsePipe(string commandLine);
#endif
