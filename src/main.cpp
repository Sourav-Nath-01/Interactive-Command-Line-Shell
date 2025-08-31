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
    read_history("shellHistory.txt");
    string prevDir;
    char startDir[PATH_MAX];
    getcwd(startDir,sizeof(startDir));
    shellStart=startDir;
    stifle_history(20);
    while(1){
        string str;
        char* input;
        char* path;
        path=strdup(shellPrompt().c_str());
        input=readline(path);
        str=input;
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
                write_history("shellHistory.txt");
                return 0;
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
            else if(args[0]=="history"){
                runHistory(args);
            }
            else if(args[0]=="pinfo"){
                runPinfo(getpid(),isBgProcess,args);
            }
            else{
                cout<<"Command not found";
            }
            

        }
        add_history(input);
        }
    return 1;
}

