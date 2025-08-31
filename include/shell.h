#ifndef SHELL_H
#define SHELL_H

#include <string>
#include <vector>
#include <dirent.h>
using namespace std;

// global
extern string shellStart;

// functions
string checkingPath(string dir, string home);
string shellPrompt();
void splitToken(string str, vector<string>& args);
void tokenize(string str, vector<string> &tokens);
void runpwd();
void runecho(vector<string> words);
void runcd(vector<string> words, string& prevDir);
void printAllDir(bool ,bool , int);
void printAll(string ,mode_t ,string );
void runls(vector<string>& );
void runSearch(vector<string>& args);
void search(string fileName);
bool searchRecursively(DIR* currPath,string targetFile);
void runBgProcess(vector<string>& args);
void runHistory(vector<string>&);
void runPinfo(pid_t pid,bool isBg,vector<string>& args);
#endif

