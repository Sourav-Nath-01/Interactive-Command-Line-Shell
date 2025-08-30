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
#include <readline/readline.h>
#include <readline/history.h>

using namespace std;

string shellStart;

string checkingPath(string dir,string home){
    bool flag=1;
    string path="";
    if(dir.size()<home.size()){
        flag=0;
    }
    for(int i=0;i<home.size();i++){
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

string shellPrompt(){
    char hostname[HOST_NAME_MAX];
    gethostname(hostname,sizeof(hostname));
    struct passwd *pw=getpwuid(getuid());
    string username=pw->pw_name;
    char cwd[PATH_MAX];
    getcwd(cwd,sizeof(cwd));
    string dir=cwd;
    string home=getenv("HOME");

    dir=checkingPath(dir,home);
    string res=username+"@"+hostname+":"+dir+"> ";
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

void tokenize(string str,vector<string> &tokens){
    char temp[str.size() + 1];
    strcpy(temp, str.c_str());
    char *token=strtok(temp,";");
    while(token!=NULL){
        string s=token;
        int start = s.find_first_not_of(" \t");
        int end   = s.find_last_not_of(" \t");
        if (start != string::npos)
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
    for(int i=1;i<words.size();i++){
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
        path=shellStart;
    }
    else if(words[1]=="-"){
        if(prevDir==""){
            cout<<"OLDPWD not set"<<endl;
        }
        else{
        path=prevDir;
        }
    }
    else{
        path=words[1];
    }
    int valid=chdir(path.c_str());  
    if(valid!=0){
        cout<<"No such file or directory";
    }
    
    prevDir=cwd;
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
    DIR *dr;
    char path[PATH_MAX];
    string finPath;
    if(fileName!=""){
        finPath=fileName;
    }
    else{
    switch (whatDir){
        case 1:{
            dr=opendir(".");
            char* check=getcwd(path,PATH_MAX);
            finPath=check;
            break;
        }
        case 2:{
            dr=opendir("..");
             char* check=getcwd(path,PATH_MAX);
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
            finPath=check;
        }
    }
    }
    if(dr==NULL){
        perror("opendir failed");
    }
    for(struct dirent *i=readdir(dr);i!=NULL;i=readdir(dr)){
        if(flagA==false && (*i).d_name[0]=='.'){
            continue;
        }
        if(flagL==false){
            cout<<(*i).d_name<<" ";
        }
        else{
            struct stat fileStat;
            stat((*i).d_name,&fileStat);
            printAll(finPath,fileStat.st_mode,(*i).d_name);
        }
    }
    cout<<endl;

}
void runls(vector<string>& args){
    bool flagA,flagL;
    flagA=false;
    flagL=false;
    int temp;
    string fileName="";
    for(int i=1;i<args.size();i++){
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
        }

    }
   
    printAllDir(flagA,flagL,temp,fileName);
   
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
    cout<<isPresent<<endl;
}
void runSearch(vector<string>& args){
    string fileName;
    for(int i=1;i<args.size();i++){
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
    }
}
//will do later
void runPinfo(pid_t pid,bool isBg){

}
//
void runHistory(vector<string>& args){
    int limit=10;
    for(int i=1;i<args.size();i++){
        if(args[i]!=" "){
            limit=min(limit,stoi(args[i]));
        }
    }
    HIST_ENTRY **hist=history_list();
    int len=history_length;
    for(int i=0;i<len && i<limit;i++){
        cout<<hist[i]->line<<endl;
    }
}