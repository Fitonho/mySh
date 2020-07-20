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
int spawn(char* program, char** argList); //spawna um processo do programa de entrada
void ignore (int signal_number);
void sigStructsInit();

char* user;
char* name;
char* home;
sig_atomic_t interrupted = 0;
int running = 1;



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
        wait(&childStatus);
        
        for(int i=0; argList[i]!=NULL;i++){
            argList[i] = NULL;
        }    
    }

    printf("finalizando...\n");
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
        interrupted = 0;
        printf("\n");
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

    if(strcmp(command,"cd") == 0){
        if( argList[1] == NULL || strcmp(argList[1],"~") == 0)
            chdir(home);
        else if (chdir(argList[1]) == -1)
            fprintf(stderr,"Error: no such directory.\n");
    }
        
    else if(strcmp(command,"exit") == 0)
        return 1;
    else
        spawn(command,argList);
    
    return 0;
}


int spawn(char* program, char** argList){
    pid_t childPid;
    
    childPid = fork();
    if(childPid != 0)
        return childPid;
    else{
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