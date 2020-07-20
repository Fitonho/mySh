#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#define MAX_CMD_SIZE 512
#define MAX_ARG_NUMBER 10

void printPromptString(); //cria e printa a Prompt String
char* parseCWD(); //se cwd for filho ou o próprio $HOME, coloca '~' no lugar do $HOME
char* getCommand(char* command,unsigned long size); //retorna stdin menos \n
char** split(char* str,char** splices); //divide str em substrings separadas por ' ' 
int runCommand(char* command, char** argList); //roda o comando se for um built-in, senão chama spawn. retorna 1 caso comando seja exit
int spawn(char* program, char** argList,int readPipe,int writePipe); //spawna um processo do programa de entrada
void ignore (int signal_number);
void sigStructsInit();
int changeDirectory(char* command, char** argList);
int getNextPipePosition(char** argList);
int handlePipes(char* command, char** argList, int readPipe);

char* user;
char* name;
char* home;
sig_atomic_t interrupted = 0;
int running = 1;
int childList[MAX_ARG_NUMBER];
int childI = 0;



int main(){
    sigStructsInit();
    user = getenv("USER");
    name = getenv("NAME");
    home = getenv("HOME");
    int childStatus;
    char* command = (char*)malloc(MAX_CMD_SIZE * sizeof(char));
    char** argList = malloc(MAX_ARG_NUMBER * sizeof(char*));

    while(running){
        
        printPromptString();
        command = getCommand(command,MAX_CMD_SIZE);
        argList = split(command,argList);
        if(runCommand(argList[0],argList)) break;
        for(int i = 0; i<childI;i++){
            waitpid(childList[i],&childStatus,0);
        }
        childI =0;
        for(int i=0; argList[i]!=NULL;i++){
            argList[i] = NULL;
        }
        command[0]='\0';
    }

    free(argList);
    free(command);
    return 0;
}

void printPromptString(){
    char* GREEN = "\033[0;32m";
    char* BLUE = "\033[0;34m";
    char* ENDCOLOR = "\033[0m";

    char* cwd = parseCWD();

    printf("[MySh]%s%s@%s:%s%s%s$ ",GREEN,user,name,BLUE,cwd,ENDCOLOR);
    free(cwd);
}

char* parseCWD(){
    char* cwd = getcwd(NULL,0);
    char* parsedCwd = malloc(strlen(cwd));
    if(strncmp(cwd,home,strlen(home)) == 0){
        parsedCwd[0]='~';
        strcpy(parsedCwd+1,cwd+strlen(home));
        free(cwd);
        return parsedCwd;
    }
    else{
        free(parsedCwd);
        return cwd;
    }    
}

char* getCommand(char* command,unsigned long size){
    fgets(command,size,stdin);
    if(strlen(command)>0)
        command[strlen(command)-1] = '\0';
    else{
        if(interrupted != 1){
            running = 0;
        } 
        printf("\n");
        interrupted = 0;
    }
        
    return command;
}

char** split(char* str,char** splices){
    char delim = ' ';
    int i;
    
    char* token = strtok(str,&delim);
    for(i=0; token!=NULL && i<MAX_ARG_NUMBER;i++){
        splices[i] = token;
        token = strtok(NULL,&delim);
    }
    splices[i]=NULL;
    free(token);
    return splices;
}

int runCommand(char* command, char** argList){
    if(*argList == NULL)
        return 0;

    if(strcmp(command,"cd") == 0)
        changeDirectory(command,argList);
        
    else if(strcmp(command,"exit") == 0)
        return 1;
    else{
        handlePipes(command,argList,0);
    }
    
    return 0;
}
int handlePipes(char* command, char** argList,int readPipe){
    int pipePos;
    if((pipePos = getNextPipePosition(argList)) != 0){
        int pipeDescriptors[2];
        pipe(pipeDescriptors);
        int readFd = pipeDescriptors[0]; int writeFd = pipeDescriptors[1];

        char** firstArg = malloc(pipePos*sizeof(char*));
        memcpy(firstArg,argList,pipePos*sizeof(char*));
        firstArg[pipePos] = NULL;

        spawn(command,firstArg,readPipe,writeFd);
        close(writeFd);
        handlePipes(argList[pipePos+1],&argList[pipePos+1],readFd);
        close(readFd);
        free(firstArg);
    }else{
        spawn(command,argList,readPipe,0);
    }
}

int getNextPipePosition(char** argList){
    for(int i=0; argList[i]!=NULL;i++){
            if(strcmp(argList[i],"|") == 0){
                return i;
            }
    }
    return 0;
}

int changeDirectory(char* command, char** argList){
    if( argList[1] == NULL || strcmp(argList[1],"~") == 0)
        chdir(home);
    else if (chdir(argList[1]) == -1)
        fprintf(stderr,"Error: no such directory.\n"); 
}


int spawn(char* program, char** argList,int readPipe,int writePipe){
    pid_t childPid;
    
    childPid = fork();
    if(childPid != 0){
        childList[childI] = childPid;
        childI++;
        return childPid;
    }
    else{
        if(readPipe != 0)
            dup2(readPipe,STDIN_FILENO);
        if(writePipe != 0)
            dup2(writePipe,STDOUT_FILENO);
        execvp(program,argList);
        fprintf(stderr,"Error: no such file or command.\n");
        abort();
    }
}

void ignore (int signal_number){
    interrupted = 1;
}
void sigStructsInit(){
    struct sigaction sINT;
    memset(&sINT,0,sizeof(sINT));
    sINT.sa_handler = &ignore;
    sigaction(SIGINT,&sINT,NULL);
    struct sigaction sSTP;
    memset(&sSTP,0,sizeof(sSTP));
    sSTP.sa_handler = &ignore;
    sigaction(SIGTSTP,&sSTP,NULL);
}