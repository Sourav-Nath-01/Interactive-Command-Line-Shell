#include<iostream>
#include<string>
#include<vector>
#include<limits.h>
#include<unistd.h>
#include "shell.h"
#include <readline/readline.h>
#include <readline/history.h>

using namespace std;

int main(){
    string prevDir;
    char startDir[PATH_MAX];
    getcwd(startDir,sizeof(startDir));
    shellStart=startDir;
    while(1){
        string str;
        shellPrompt();
        getline(cin,str);
        vector<string> tokens;
        tokenize(str,tokens);
        bool isBgProcess=false;
        for(string tokenizestr:tokens){
            vector<string> args;
            splitToken(tokenizestr,args);
            for(string it:args){
                if(it=="&"){
                    isBgProcess=true;
                }
            }
            if(args[0]=="exit"){
                break;
            }
            else if(args[0]=="pwd"){
                runpwd();
            }
            else if(args[0]=="echo"){
                runecho(args);
            }
            else if(args[0]=="cd"){
                runcd(args,prevDir);
            }
            else if(args[0]=="ls"){
                runls(args);
            }
            else if(args[0]=="search"){
                runSearch(args);
            }
            else if(isBgProcess==true){
                runBgProcess(args);
            }
            else{
                cout<<"Command not found";
            }
        }
    }
    return 1;
}

