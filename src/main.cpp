#include<iostream>
#include<string>
#include<vector>
#include<limits.h>
#include<unistd.h>
#include "shell.h"
#include<sys/types.h>
#include <readline/readline.h>
#include <readline/history.h>
#include<signal.h>
pid_t fgpid = 0; 
using namespace std;
void handleSigint(int sig) {
    if (fgpid != 0) {
        kill(fgpid, SIGINT);
    }
}

void handleSigtstp(int sig) {
    if (fgpid != 0) {
        kill(fgpid, SIGTSTP);
        cout << "\nProcess stopped and pushed to background\n";
    }
}
char* command_generator(const char* text, int state) {
    static vector<string> matches;
    static size_t index;

    if (state == 0) { // first call
        matches = autoComplete(text); // use your function
        index = 0;
    }

    if (index < matches.size()) {
        return strdup(matches[index++].c_str());
    }
    return nullptr; // no more matches
}

char** myCompletion(const char* text, int start, int end) {
    if (start == 0) {
        return rl_completion_matches(text, command_generator);
    }
    return nullptr;
}

int main(){
    read_history("shellHistory.txt");
    string prevDir;
    char startDir[PATH_MAX];
    getcwd(startDir,sizeof(startDir));
    shellStart=startDir;
    prevDir=shellStart;
    stifle_history(20);
    signal(SIGINT, handleSigint);   // CTRL-C
    signal(SIGTSTP, handleSigtstp); // CTRL-Z
    rl_attempted_completion_function = myCompletion;
        while(1){
        string str;
        char* input;
            char* path;
            path=strdup(shellPrompt().c_str());
            input=readline(path);
            free(path);
            if(input==NULL){
                cout << "exiting shell" << endl;
                write_history("shellHistory.txt");
                return 0;
            }
            add_history(input);
            str=input;
            vector<string> tokens;
            tokenize(str,tokens);
            bool isBgProcess=false;
            bool isIOdirection=false;
            bool isPipeline=false;
            for(string tokenizestr:tokens){
            vector<string> args;
            splitToken(tokenizestr,args);
            for(string it:args){
                if(it=="&"){
                    isBgProcess=true;
                }
                if(it=="|"){
                    isPipeline=true;
                }
            }
            if(isPipeline){
                runPipeline(str,isBgProcess,fgpid,prevDir);
            }
            if(isBgProcess==false){
                runForeGroundProcess(args,prevDir);
            }
            else{
                runBgProcess(args);
            }

        }
        add_history(input);

        }
    return 1;
}

