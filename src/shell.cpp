#include<iostream>
#include<unistd.h>
#include<pwd.h>
#include<string>
#include<limits.h>
#include<string.h>
#include<vector>
#include <dirent.h>
#include <sys/stat.h>
#include "shell.h"
#include<grp.h>
#include<fcntl.h>
#include<sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
using namespace std;

string shellStart;
extern pid_t fgpid;
string checkingPath(string dir,string home){
    bool flag=1;
    string path="";
    if(dir.size()<home.size()){
        flag=0;
    }
    for(size_t i=0;i<home.size();i++){
        if(dir[i]!=home[i]){
            flag=0;
        }
    }
    if(flag==1){
        path+="~"+dir.substr(home.size());
    }
    else{
        path=dir;
    }
    return path;
}

string shellPrompt(string rootCheck){
    char hostname[HOST_NAME_MAX];
    gethostname(hostname,sizeof(hostname));
    struct passwd *pw=getpwuid(getuid());
    string username=pw->pw_name;
    char cwd[PATH_MAX];
    getcwd(cwd,sizeof(cwd));
    string dir=cwd;
    if(rootCheck==dir){
        dir="~";
    }
    string home=getenv("HOME");

    dir=checkingPath(dir,home);
    string res = string("\033[32m") + username + "@" + hostname +
             "\033[34m:" + dir +
             "\033[0m> ";
    return res;
}

void splitToken(string str,vector<string>& args){
    char temp[str.size()+1];
    strcpy(temp,str.c_str());
    char *token=strtok(temp," \t");
    while(token){
        string s=token;
        if(s.empty()==false)
            args.push_back(token);
        token=strtok(NULL," \t");
    }
}

void splitTokenByPipe(string str,vector<string>& args){
    char temp[str.size()+1];
    strcpy(temp,str.c_str());
    char *token=strtok(temp,"|");
    while(token){
        string s=token;
        s.erase(0, s.find_first_not_of(" \t"));
        s.erase(s.find_last_not_of(" \t") + 1);

        if(s.empty()==false)
            args.push_back(s);
        token=strtok(NULL,"|");
    }
}
void tokenize(string str,vector<string> &tokens){
    char temp[str.size() + 1];
    strcpy(temp, str.c_str());
    char *token=strtok(temp,";");
    while(token!=NULL){
        string s=token;
        int start = s.find_first_not_of(" \t");
        int end   = s.find_last_not_of(" \t");
        if (start != (int) string::npos)
            s = s.substr(start, end - start + 1);

        tokens.push_back(s);
        vector<string> args;
        
        token=strtok(NULL,";");
    }
}

void runpwd(){
    char cwd[PATH_MAX];
    char* temp=getcwd(cwd,sizeof(cwd));
    if(temp==NULL){
        perror("get cwd failed");
    }
    else{
        cout<<cwd<<endl;
    }
}

void runecho(vector<string> words){
    for(size_t i=1;i<words.size();i++){
        cout<<words[i]<<" ";
    }
    cout<<endl;
}

void  runcd(vector<string> words,string& prevDir){
    char cwd[PATH_MAX];
    getcwd(cwd,sizeof(cwd));
    
    string path;
    if(words.size()>2){
        cout<<"Invalid Arguements"<<endl;
    }
    else if((words.size()==1) || (words[1]=="~")){
        path=getenv("HOME");
    }
    else if(words[1]=="-"){
        if(prevDir==""){
            cout<<"OLDPWD not set"<<endl;
            return;
        }
        else{
        path=prevDir;
        }
    }
    else{
        path=words[1];
    }
    prevDir=cwd;
    int valid=chdir(path.c_str());  
    if(valid!=0){
        perror("No such file or directory");
    }
    return;
}
void printAll(string path,mode_t mode,string filename) {
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) == -1) {
        perror("stat failed");
        return;
    }
    cout << (S_ISDIR(mode) ? 'd' : '-');
    cout << ((mode & S_IRUSR) ? 'r' : '-');
    cout << ((mode & S_IWUSR) ? 'w' : '-');
    cout << ((mode & S_IXUSR) ? 'x' : '-');
    cout << ((mode & S_IRGRP) ? 'r' : '-');
    cout << ((mode & S_IWGRP) ? 'w' : '-');
    cout << ((mode & S_IXGRP) ? 'x' : '-');
    cout << ((mode & S_IROTH) ? 'r' : '-');
    cout << ((mode & S_IWOTH) ? 'w' : '-');
    cout << ((mode & S_IXOTH) ? 'x' : '-');
    cout << " " << fileStat.st_nlink;


    struct passwd *psd = getpwuid(fileStat.st_uid);
    struct group  *gr =  getgrgid(fileStat.st_gid);

    if(psd!=NULL){
        cout<<psd->pw_name;
    }
    else{
        cout<<"Unknown user";
    }
    if(gr!=NULL){
        cout<<gr->gr_name;
    }
    else{
        cout<<"Unknown Group";
    }
    

    cout << " " << fileStat.st_size;
    char timeStore[80];
    struct tm *time = localtime(&fileStat.st_mtime);
    strftime(timeStore, sizeof(time), "%b %d %H:%M", time);
    cout << " " << timeStore;

    cout << " " << filename << endl;
}

void printAllDir(bool flagA,bool flagL,int whatDir,string fileName){
    //assuming whatDir = 1 means curr directory
    // 2 means parent directory
    // 3 means root directory
    DIR *dr=NULL;
    char path[PATH_MAX];
    string finPath;
    if(fileName!=""){
        dr=opendir(fileName.c_str());
        finPath=fileName;
    }
    else{
    switch (whatDir){
        case 1:{
            dr=opendir(".");
            char* check=getcwd(path,PATH_MAX);
            if(check==NULL){
                perror("getcwd failed");
            }
            finPath=check;
            break;
        }
        case 2:{
             dr=opendir("..");
             char* check=getcwd(path,PATH_MAX);
             if(check==NULL){
                perror("getcwd failed");
             }
             finPath=string(path)+"/..";
             break;
        }
        case 3:{
            const char *startPath=shellStart.c_str();
            dr=opendir(startPath);
            finPath=startPath;
            break;
        }
        default:{
            dr=opendir(".");
            char* check=getcwd(path,PATH_MAX);
            if(check==NULL){
                perror("getcwd failed");
            }
            finPath=check;
        }
    }
    }
    if(dr==NULL){
        perror("opendir failed");
        return;
    }
    if(fileName!="." && fileName!=".." && fileName!="~" && fileName!=" " && fileName!=""){
        cout<<fileName<<":"<<endl;
    }
    
    for(struct dirent *i=readdir(dr);i!=NULL;i=readdir(dr)){
        if(flagA==false && (*i).d_name[0]=='.'){
            continue;
        }
        if(flagL==false){
            cout<<(*i).d_name<<"  ";
        }
        else{
            struct stat fileStat;
            stat((*i).d_name,&fileStat);
            printAll(finPath,fileStat.st_mode,(*i).d_name);
        }
    }
    cout<<endl;
    return;
}
void runls(vector<string>& args){
    bool flagA,flagL;
    flagA=false;
    flagL=false;
    int temp;
    string fileName="";
    bool isPrintedBefore=false;
    for(size_t i=1;i<args.size();i++){
        if(args[i]=="-l"){
            flagL=true;
        }
        else if(args[i]=="-a"){
            flagA=true;
        }
        else if((args[i]=="-la") || (args[i]=="-al")){
            flagL=true;
            flagA=true;
        }
        if(args[i]=="."){
            temp=1;
        }
        else if(args[i]==".."){
            temp=2;
        }
        else if(args[i]=="~"){
            temp=3;
        }
        if(args[i]!="." && args[i]!=".." && args[i]!="~" && args[i]!=" " && args[i]!="-a" && args[i]!="-l" && args[i]!="-al" && args[i]!="-la"){
            fileName=args[i];
            printAllDir(flagA,flagL,temp,fileName);
            flagA=false;
            flagL=false;
            isPrintedBefore=true;
        }

    }
    if(isPrintedBefore==false){
    printAllDir(flagA,flagL,temp,fileName);
    }
   
}
bool searchRecursively(DIR* currPath,string targetFile,string parentPath){
    
    for(struct dirent *dt=readdir(currPath);dt!=NULL;dt=readdir(currPath)){
       string check=dt->d_name;
        if(check=="." || check==".."){
            continue;
        }
        
        if((*dt).d_type==DT_DIR){
            string newParPath=(parentPath+"/"+dt->d_name);
             DIR* newDt=opendir(newParPath.c_str());
             if(newDt==NULL){
                cout<<"opendir failed"<<endl;
                return false;
             }
             if(searchRecursively(newDt,targetFile,newParPath)==true){
                return true;
             }
        }
        else{
            if(targetFile==dt->d_name){
                return true;
            }
        }
    }
    return false;
}
void search(string fileName){
    DIR *currPath=opendir(".");
    bool isPresent=searchRecursively(currPath,fileName,".");
    if(isPresent==1){
        cout<<"True"<<endl;
    }
    else{
        cout<<"False"<<endl;
    }
}
void runSearch(vector<string>& args){
    string fileName;
    for(size_t i=1;i<args.size();i++){
        if(args[i]!=" "){
            fileName=args[i];
        }
    }
    search(fileName);
}

void runBgProcess(vector<string>& args){
    pid_t pid=fork();
    if(pid<0){
        perror("fork failed");
    }
    if(pid>0){
        //parent process
        cout<<getpid()<<endl;
        return;
    }
    else{
        //child process
        int n=args.size();
        char* newArgs[n+1];
        for(int i=0;i<n;i++){
            newArgs[i]=const_cast<char*>(args[i].c_str());
        }
        newArgs[n]=NULL;
        execvp(newArgs[0],newArgs);
        perror("execvp failed");
        exit(1);
    }
}
void runForeGroundProcess(vector<string> args,string& prevDir){
    if (args[0] == "cd") {
        runcd(args, prevDir);
        return;
    }
    if (args[0] == "exit") {
         /*string histFile = shellStart + "/shellHistory.txt";
        write_history(histFile.c_str());*/
        write_history("shellHistory.txt");
        exit(0);
    }
    fgpid=fork();
    if(fgpid<0){
        perror("fork failed");
    }
    if(fgpid>0){
        //parent process
        int status;
        waitpid(fgpid,&status,WUNTRACED);
        fgpid=0;
        return;
    }
    else{
        //child process
            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            if(args[0]=="exit"){
                write_history("shellHistory.txt");
                exit(0);
            }
            else if(args[0]=="pwd" || args[0]=="echo" || args[0]=="ls" || args[0]=="history" || args[0]=="pinfo"){
                runCommand(args,false,getpid());
            }
            else if(args[0]=="search"){
                runSearch(args);
            }
            else{
             
               ioRedirection(args);
            }
            exit(0);
    }
}
//will do later
void runPinfo(pid_t pid,bool isBg,vector<string>& args){
    for(size_t i=1;i<args.size();i++){
        pid=stoi(args[i]);
    }
    string strPid=to_string(pid);
    string path="/proc/"+strPid+"/stat";
    string exePath="/proc/"+strPid+"/exe";
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        perror("open failed");
        return;
    }

    char buffer[4096];
    int bytes = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes < 0) {
        perror("read failed");
        close(fd);
        return;
    }
    buffer[bytes] = '\0';
    close(fd);

    char* token = strtok(buffer, " ");
    int field = 1;
    char status = '?';
    unsigned long vsize = 0;

    while (token) {
        if (field == 3){
            status = token[0];  
        }             
        if (field == 23){
            vsize = strtoul(token, NULL, 10); 
        } 
        token = strtok(NULL, " ");
        field++;
    }
    char exePathStore[PATH_MAX];
    int len = readlink(exePath.c_str(), exePathStore, sizeof(exePathStore) - 1);
    if (len != -1) {
        exePathStore[len] = '\0';
    } else {
        strcpy(exePathStore, "Executable path not found");
    }

    string statusStr(1, status);
    if (isBg == false) {   // assume false means foreground
        statusStr += "+";
    }

    // ---- Print info ----
    cout << "Process Status -- " << statusStr << "\n";
    cout << "memory -- " << vsize << "\n";
    cout << "Executable Path -- " << exePathStore << "\n";
}
//
void runHistory(vector<string>& args){
    int limit=10;
    for(size_t i=1;i<args.size();i++){
        if(args[i]!=" "){
            limit=min(limit,stoi(args[i]));
        }
    }
    HIST_ENTRY **hist=history_list();
    int len=history_length;
    int start=max(0,len-limit);
    for(int i=start;i<len;i++){
        cout<<hist[i]->line<<endl;
    }
}

void    runUsingExecv(vector<string>& args){
    string inputFile,outputFile,appendFile;
    for(size_t i=0;i<args.size();i++){
        if(args[i].empty()==false && args[i].front()=='"'){
            args[i]=args[i].substr(1);
        }
        if(args[i].empty()==false && args[i].back()=='"'){
            args[i]=args[i].substr(0,args[i].size()-1);
        }
        if(args[i]=="<" && ((i+1)<args.size())){
            inputFile=args[i+1];
            i++;
        
        }
        if(args[i]==">" && ((i+1)<args.size())){
            outputFile=args[i+1];
            i++;
     
        }
        if(args[i]==">>" && ((i+1)<args.size())){
            appendFile=args[i+1];
            i++;
        
        }
        
    } 
       
        if(inputFile.empty()==false){
            int srcFd=open(inputFile.c_str(),O_RDONLY );
            dup2(srcFd,STDIN_FILENO);
            close(srcFd);
        }
        if(outputFile.empty()==false){
            int targetFd=open(outputFile.c_str(),O_WRONLY | O_TRUNC | O_CREAT,0644);
            dup2(targetFd,STDOUT_FILENO);
            close(targetFd);
        }
        if(appendFile.empty()==false){
            int appendFd=open(appendFile.c_str(),O_WRONLY | O_CREAT  | O_APPEND ,0644);
            dup2(appendFd,STDOUT_FILENO);
            close(appendFd);
        }

        
        vector<char*> newargs;
        size_t j;
        for(j=0;j<args.size();j++){
            
            if(args[j]=="<" || args[j]==">" || args[j]==">>"){
                j+=1;
                continue;
            }
            else{
            newargs.push_back(const_cast<char*>(args[j].c_str()));
            }
        }
        newargs.push_back(nullptr);
        execvp(newargs[0],newargs.data());
        perror("execv failed");
        exit (1);
    }
void runCommand(vector<string>& args,bool isBgProces,pid_t pid){
    string outputFile,appendFile;
    int saved_stdout = dup(STDOUT_FILENO);
    for(size_t i=0;i<args.size();i++){
        if(args[i]=="<" && ((i+1)<args.size())){
            fprintf(stderr,"Wrong Syntax");
            return ;
            i++;
        
        }
        if(args[i]==">" && ((i+1)<args.size())){
            outputFile=args[i+1];
            i++;
     
        }
        if(args[i]==">>" && ((i+1)<args.size())){
            appendFile=args[i+1];
            i++;
        
        }
    }
        vector<string> newArgs;
        for(size_t i=0;i<args.size();i++){
            if(args[i]=="<" || args[i]==">" || args[i]==">>"){
                i++;
                continue;
            }
            newArgs.push_back(args[i]);
        }
       
   
        if(outputFile.empty()==false){
            int targetFd=open(outputFile.c_str(),O_WRONLY | O_TRUNC | O_CREAT,0644);
            dup2(targetFd,STDOUT_FILENO);
            close(targetFd);
        }
        if(appendFile.empty()==false){
            int appendFd=open(appendFile.c_str(),O_WRONLY | O_CREAT  | O_APPEND ,0644);
            dup2(appendFd,STDOUT_FILENO);
            close(appendFd);
        }
        if(args[0]=="ls" ){
            runls(newArgs);
        }
        else if(args[0]=="pwd"){
            runpwd();
        }
        else if(args[0]=="pinfo"){
            runPinfo(pid,isBgProces,newArgs);
        }
        else if(args[0]=="history"){
            runHistory(newArgs);
        }
        else if(args[0]=="echo"){
            runecho(newArgs);
        }
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
}

void ioRedirection(vector<string>& args){

    pid_t pid=fork();
    if(pid<0){
        perror("fork failed");
        return;
    }
    if(pid>0){
        //parent process
        waitpid(pid,NULL,0);
    }
    else{
        //child process 
            runUsingExecv(args);
    }
}
void runPipeline(string input) {
    // Step 1: Split input by pipe '|'

    vector<vector<string>> args;
    vector<string> tempArgs;
    splitTokenByPipe(input, tempArgs);
    for (auto &cmdStr : tempArgs) {
        vector<string> tokens;
        splitToken(cmdStr, tokens);
        args.push_back(tokens);
    }

    int n = args.size();
    if (n == 0) return; // handle empty input
    if (n == 1) {       // single command, no pipe
        pid_t pid = fork();
        if (pid == 0) {
            vector<string> newArgs = args[0];

            int in_fd, out_fd;
            for (size_t k = 0; k < newArgs.size();) {
                if (newArgs[k] == "<") {
                    in_fd = open(newArgs[k+1].c_str(), O_RDONLY);
                    if (in_fd < 0) { perror("open input failed"); exit(1); }
                    dup2(in_fd, STDIN_FILENO);
                    close(in_fd);
                    newArgs.erase(newArgs.begin() + k, newArgs.begin() + k + 2);
                } else if (newArgs[k] == ">") {
                    out_fd = open(newArgs[k+1].c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (out_fd < 0) { perror("open output failed"); exit(1); }
                    dup2(out_fd, STDOUT_FILENO);
                    close(out_fd);
                    newArgs.erase(newArgs.begin() + k, newArgs.begin() + k + 2);
                } else {
                    k++;
                }
            }
            // --------------------------------------------------------------------------

            char* newArgv[newArgs.size() + 1];
            for (size_t k = 0; k < newArgs.size(); k++) newArgv[k] = const_cast<char*>(newArgs[k].c_str());
            newArgv[newArgs.size()] = NULL;
            execvp(newArgv[0], newArgv);
            perror("exec failed");
            exit(1);
        }
        wait(NULL);
        return;
    }

    int pipes[n-1][2];

    for (int i = 0; i < n-1; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("pipe failed");
            exit(1);
        }
    }

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }

            if (i < n-1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            for (int j = 0; j < n-1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            vector<string> newArgs = args[i];

            int in_fd, out_fd;
            for (size_t k = 0; k < newArgs.size();) {
                if (newArgs[k] == "<") {
                    in_fd = open(newArgs[k+1].c_str(), O_RDONLY);
                    if (in_fd < 0)
                     {
                         perror("open input failed"); 
                         exit(1);
                     }
                    dup2(in_fd, STDIN_FILENO);
                    close(in_fd);
                    newArgs.erase(newArgs.begin() + k, newArgs.begin() + k + 2);
                } else if (newArgs[k] == ">") {
                    out_fd = open(newArgs[k+1].c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (out_fd < 0) 
                    {
                         perror("open output failed"); exit(1);
                    }
                    dup2(out_fd, STDOUT_FILENO);
                    close(out_fd);
                    newArgs.erase(newArgs.begin() + k, newArgs.begin() + k + 2);
                } else {
                    k++;
                }
            }
         
            char* newArgv[newArgs.size() + 1];
            for (size_t k = 0; k < newArgs.size(); k++)
                 newArgv[k] = const_cast<char*>(newArgs[k].c_str());
            newArgv[newArgs.size()] = NULL;

            execvp(newArgv[0], newArgv);
            perror("exec failed");
            exit(1);
        }
    }

    for (int i = 0; i < n-1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

   
    for (int i = 0; i < n; i++) wait(NULL);
}


vector<string> autoComplete(string str){
    vector<string> commands={"cd","exit","pwd","echo","ls","history","pinfo","search"};
    vector<string> matchedList;
    for(size_t i=0;i<commands.size();i++){
        bool isMatch=true;
        size_t j=0;
        for(;j<str.size() && j<commands[i].size();j++){
            if(str[j]!= commands[i][j]){
                isMatch=false;
                break;
            }
        }
        if(isMatch && j==str.size()){
            matchedList.push_back(commands[i]);
        }
    }
    return matchedList;
}