#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>

#include "imprimer.h"
#include <readline/readline.h>
#include "myh.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
static int map[64][64]={0};
static char* files[MAXFILENUM]={0};
static PRINTER printers[MAXFILENUM];
static JOB jobs[MAXJOBNUM];
static int visited[MAXFILENUM]={-1};
static CONVERSIONS conversionMap[MAXFILENUM][MAXFILENUM];
static int numOfFile=0;
static int numOfPrinter=0;
static int numOfJobs=0;
//static int numOfJObs=0;
CONVERSIONS* conversion;
//int makeConversion(int x, int y,CONVERSIONS conversion_program);
//PRINTER* printers[MAX_PRINTERS];
//CONVERSION* conversion[]
/*
 * "Imprimer" printer spooler.
 */

//static int map[64][64]={0};
int main(int argc, char *argv[])
{
    //signal(SIGCHLD,handler);
    char optval;
    int i=0;
    FILE* file;
    FILE* out_file;
    while(optind < argc) {
	if((optval = getopt(argc, argv, "i:o:")) != -1) {
	    switch(optval) {
        case 105:
            file=fopen(optarg,"r");
            i=1;
        break;
        case 111:
            out_file=fopen(optarg,"w");
            stdout=out_file;
            break;
	    case '?':
		fprintf(stderr, "Usage: %s [-i <cmd_file>] [-o <out_file>]\n", argv[0]);
		exit(EXIT_FAILURE);
		break;
	    default:
		break;
	    }
	}
    }
    
    while(1)
    {
        int firstCMD=0;
        //char buf[64];
        char* token;
        int len=0;
        char line[100];

        char* cmd;
        if(i==1)
        {

            if(fgets(line,sizeof(line),file)!=NULL)
            {
                token=line;
                len=strlen(line);
                if(token[len-1]=='\n')
                {
                    token[len-1]=0;
                }
                token=strtok(token," ");
                firstCMD=convertToInt(token);
            }
            else{
                i=0;
            cmd=readline("imp>");
            token=strtok(cmd," ");
            firstCMD=convertToInt(token);}
        }
        else
        {
            cmd=readline("imp>");
            token=strtok(cmd," ");
            firstCMD=convertToInt(token);
        }
                switch(firstCMD)
                {
                    case HELP:
                        print_help();
                        break;
                    case QUIT:
                        exit(EXIT_SUCCESS);
                        break;
                    case INVALID:
                        //fprintf(stdout,"%s",imp_format_error_message("invalid commend\n",buf,sizeof(buf)));
                        break;
                    case TYPE:
                        creatType(token);
                        break;
                    case PRINTER1:
                        creatPrinter(token);
                        break;
                    case CONVERSION:
                        creatConversion(token);
                        break;
                    case PRINTERSS:
                        printP();
                        break;
                    case JOBS:
                        printJ();
                        break;
                    case PRINT:
                        creatPrint(token);
                        break;
                    case ENABLE:
                        enablePrinter(token);
                    
                        break;
                    default:
                        break;
                }
    }
    exit(EXIT_SUCCESS);
}
// void handler(int signal)
// {

// }
int enablePrinter(char* token)
{
    token=strtok(NULL," ");
    int id;
    if((id=testPrinter(token))>=0)
    {
        // printers[id].busy
        upPrinter(id,1,printers[id].busy);
        return 0;
    }
    return -1;
}
//return printer ID
int testPrinter(char* name)
{
    for(int i=0;i<numOfPrinter;i++)
    {
        if(!strcmp(name,printers[i].name))
        {
            return i;
        }
    }
    return -1;
}
int creatJobs(int numOfJobs,char* name)
{
    jobs[numOfJobs].jobid=numOfJobs;//id
    jobs[numOfJobs].pgid=0;
    jobs[numOfJobs].file_name=malloc(sizeof(name));
    strcpy(jobs[numOfJobs].file_name,name);
    char* file_type;
    file_type=strtok(name,".");
    file_type=strtok(NULL," ");
    jobs[numOfJobs].file_type=malloc(sizeof(file_type));
    strcpy(jobs[numOfJobs].file_type,file_type);
    jobs[numOfJobs].chosen_printer=NULL;
    jobs[numOfJobs].status=QUEUED;
    gettimeofday(&jobs[numOfJobs].change_time,NULL);
    //upJobs(numOfJobs,QUEUED);
    gettimeofday(&jobs[numOfJobs].creation_time,NULL);
    //numOfJobs++;
    return 1;
}
int upJobs(int id,JOB_STATUS status)
{
    jobs[id].status=status;
    gettimeofday(&jobs[id].change_time,NULL);
    my_imp_format_job_status(id);
    //call upprinter
    return 1;
}
int printJ()
{
    char *buf=malloc(128);
    int size=128;
    for(int i=0;i<numOfJobs;i++)
    {  
        fprintf(stdout,"%s\n",imp_format_job_status(&jobs[i],buf,size));
    }
    return 1;
    free(buf);
}
int printP()
{
    char *buf=malloc(64);
    int size=64;
    for(int i=0;i<numOfPrinter;i++)
    {
        fprintf(stdout,"%s\n",imp_format_printer_status(&printers[i],buf,size));
    }
    return 1;
    free(buf);
}
// void
// pipeline(char ***cmd)
void pipeline(char ***cmd,JOB *job)
{
    //main process
    int pid=fork();
    if(pid==-1)
    {
        my_imp_format_error_message("main process error");
    }
    if(pid==0)//master process child
    {
        int fd[2];
        int fdd = 0;
        int index=0;
        job->pgid=getpid();
        setpgid(getpid(),0);
        upJobs(job->jobid,RUNNING);
        int file=open(job->file_name,O_RDONLY);
        dup2(file,0);
        close(file);
        while(cmd[index]!=NULL){
        pipe(fd);               /* Sharing bidiflow */
        int ccpid;
        // dup2(file,fdd);
        if ((ccpid = fork()) == -1) {
            my_imp_format_error_message("fork error");

        }
        else if (ccpid == 0) {
            if(index!=0){
                dup2(fdd, 0);
            }
            if(*(cmd + 1) != NULL){
                dup2(fd[1], 1);
                close(fd[0]);
            }
            else{
                int printerFD=imp_connect_to_printer(job->chosen_printer,PRINTER_NORMAL);
                dup2(printerFD,1);
                close(printerFD);
            }
            close(fd[0]);
            if(execvp((*cmd)[0], *cmd)==-1)
            {
                exit(0);
            }
            my_imp_format_error_message("fork exit");
            }
            else {
            int status;
            waitpid(ccpid,&status,WCONTINUED|WNOHANG|WUNTRACED);
            
                if(WIFEXITED(status))
                {
                    if(WEXITSTATUS(status)==1)
                    {
                        // exit(1);
                        my_imp_format_error_message("fork exit");
                    }
                }
            
            close(fd[1]);
            fdd = fd[0];
            cmd++;
            }
        }
        exit(0);
    }
    if(pid>0)//master parent
    {
        int status;
        waitpid(pid,&status,WCONTINUED|WNOHANG|WUNTRACED);
            if(WIFEXITED(status))
            {
             if(WEXITSTATUS(status)==1)
                {
                    //exit(1);
                    my_imp_format_error_message("fork exit");
                }
            }
        
    }
    //exit(1);
    return;
}

// void printFiles()
// {
//     for(int i=0;i<numOfFile;i++)
//     {
//         printf("%d is %s\n", i,files[i]);
//     }
// }
// void printMap()
// {
//     for(int i=0; i<numOfFile;i++)
//     {
//         for(int j=0;j<numOfFile;j++)
//         {
//             printf("%d,",map[i][j]);
//         }
//         printf("\n");
//     }
// }
//only upprinter and creatJob can call this
int assignnJob(int printerID)
{
    if(printerID>=0)//given printer find job
    {
        for(int i=0; i<numOfJobs; i++)
        {
            if(jobs[i].status==QUEUED)//find waiting job
            {   
                if((jobs[i].eligible_printers&(1<<printerID))==1)// printer inside jobs
                {   
                    int depth;
                    if((depth=convertable(getTypeID(jobs[i].file_type),getTypeID(printers[printerID].type)))>=0)//path exist
                    {
                        jobs[i].chosen_printer=&printers[printerID];//link printer to job
                        upPrinter(printerID,1,1);//printer enabled and busy
                        
                        getPath(&jobs[i]);
                        return 0; //success                     
                    }
                }
            }
        }
    }
    return -1;//fail  
}
void getPath(JOB *job)
{
    int depth=-1;
    char* start=job->file_type;
    char* target=((PRINTER*)(job->chosen_printer))->type;
    depth=convertable(getTypeID(start),getTypeID(target));
    depth=depth+2;
    char** cmd[depth];
    int from=-1,to=-1;
    int index=0;
    for(int i=0; i<numOfFile;i++)
    {
        if(i==0)
        {
            to=visited[i];
        }
        else
        {
            if(visited[i]!=-1)
            {
                from=to;
                to=visited[i];
                int size=conversionMap[from][to].numOfArgs+2;
                cmd[index]=malloc(sizeof(char**)*depth);
                cmd[index][0]=malloc(sizeof(char)*20);
                cmd[index][0]=conversionMap[from][to].conversion_program;
                for(int i=1;i<size-1;i++)
                {
                    cmd[index][i]=malloc(sizeof(char)*20);
                    cmd[index][i]=conversionMap[from][to].args[i-1];
                }
                cmd[index][size-1]=NULL;
                index++;
            }
            else if(index<depth-1)
            {
                cmd[index]=malloc(sizeof(char**)*depth);
                cmd[index][0]=malloc(sizeof(char)*20);
                cmd[index][0]="/bin/cat";
                cmd[index][1]=NULL;
                cmd[index+1]=NULL;
                index++;
            }
        }
        
    }
    pipeline(cmd,job);

}
int convertable(int start, int target)
{
    initVisited();
    int depth=0;
    if((depth=bfs(start, target, depth))!=-1);
        return depth;
    return -1;
}
int bfs(int start, int target, int depth)
{
    if(map[start][target]==1)
    {
        visited[depth++]=start;
        visited[depth]=target;
        return depth;
    }
    for(int i=0;i<numOfFile;i++)
    {
        if((map[start][i]==1)&&(start!=i))
        {
            visited[depth++]=start;
            
            if((depth=bfs(i,target,depth))!=-1)
            {
                return depth;
            }
            visited[depth--]=-1;
        }
    }
    return -1;
}
int initVisited()
{
    for(int i=0;i<MAXFILENUM;i++)
    {
        visited[i]=-1;
    }
    return 1;
}
int creatType(char* token)
{
    token = strtok(NULL, " ");
    char* name=token;
    if(name!=NULL){
    //files[numOfFile]=name;
    files[numOfFile]=(char*)malloc(sizeof(char)*20);
    strcpy(files[numOfFile],name);
    map[numOfFile][numOfFile]=0;
    numOfFile++;
    return 0;
    }
    my_imp_format_error_message("type name null");
    return -1;
}
int creatConversion(char* token)
{
    token = strtok(NULL, " ");
    file_type1=token;
    token = strtok(NULL, " ");
    file_type2=token;
    token = strtok(NULL, " ");
    int start=getTypeID(file_type1);
    int target=getTypeID(file_type2);
    if((start!=-1)&&(target!=-1))
    {
        conversionMap[start][target].conversion_program=malloc(sizeof(token));
        strcpy(conversionMap[start][target].conversion_program,token);
        token = strtok(NULL, " ");
        int index=0;
        while(token!=NULL)
        {
            conversionMap[start][target].args[index]=malloc(sizeof(token));
            strcpy(conversionMap[start][target].args[index],token);
            token = strtok(NULL, " ");
            index++;
        }
        conversionMap[start][target].numOfArgs=index;
        makeMap(start, target);
        return 0;//correct
    }
    my_imp_format_error_message("conversion args invaild");
    return -1;//error
 }
int creatPrint(char* token)
{
    token=strtok(NULL, " ");
    char* name=token;
    int id=-1;
    while((token=strtok(NULL, " "))!=NULL)
    {
        id=testPrinter(token);
        if((id=testPrinter(token))!=-1)
        {
            jobs[numOfJobs].eligible_printers|=(1<<id);//assigned printer
        }
    }
    if(id==-1)
    {
        jobs[numOfJobs].eligible_printers|=ANY_PRINTER;//any printer
    }
    creatJobs(numOfJobs,name);
    numOfJobs++;

    //getPath();
    return 0;
}
int makeMap(int start, int target)
{
    map[start][target]=1;
    return 1;
}
int getTypeID(char* type)
{
    for(int i=0;i<numOfFile;i++)
    {
        if(!strcmp(type,files[i]))
        {
            return i;
        }
    }
    return -1;
}
int creatPrinter(char* token)
{
    token = strtok(NULL, " ");
    char* name=token;
    token = strtok(NULL, " ");
    char* type=token;
    if((name!=NULL)&(token!=NULL)){
    printers[numOfPrinter].id=numOfPrinter;
    printers[numOfPrinter].name=malloc(sizeof(char)*40);
    strcpy(printers[numOfPrinter].name,name);
    printers[numOfPrinter].type=malloc(sizeof(char)*40);
    strcpy(printers[numOfPrinter].type,type);
    printers[numOfPrinter].enabled=0;
    printers[numOfPrinter].busy=0;
    numOfPrinter++;
    return 0;
    }
    my_imp_format_error_message("printer args invaild");
    return -1;
}
int upPrinter(int id,int enabled,int busy)
{
    printers[id].enabled=enabled;
    printers[id].busy=busy;
    char buf[128];
    int size=128;
    fprintf(stdout, "%s\n", imp_format_printer_status(&printers[id],buf,size));
    if((printers[id].enabled==1)&(printers[id].busy!=1))// Printer not busy and enabled
    {
        assignnJob(printers[id].id);

    }
    return 0;
}
char* getFileType(char* printer)
{
    char* token=strtok(printer,".");
    token=strtok(NULL,".");
    return token;
}
char* getFileName(char* printer)
{
    char* token=strtok(printer,".");
    return token;
}
int getFileID(char* printer)
{
    int i=0;
    for(i=0;i<MAXFILENUM;i++)
    {
        if(!strcmp(printer,files[i]))
        {
            return i;
        }
    }
    return -1;
}
int my_imp_format_printer_status(int printerID)
{
    char buf[128];
    int size=128;
    fprintf(stdout, "%s\n", imp_format_printer_status(&printers[printerID],buf,size));
    return 1;
}
int my_imp_format_job_status(int jobID)
{
    char buf[128];
    int size=128;
    fprintf(stdout, "%s\n", imp_format_job_status(&jobs[jobID],buf,size));
    return 1;
}
int my_imp_format_error_message(char* mes)
{
    char buf[128];
    int size=128;
    fprintf(stdout, "%s\n", imp_format_error_message(mes,buf,size));
    return 1;
}
void print_help()
{
    fprintf(stdout,"help:\n%s",help);
}
int convertToInt(char* first)
{
    if(first!=NULL){
    if(!strcmp(first,"help"))
    {
        return 0;
    }
    if(!strcmp(first,"quit"))
    {
        return 1;
    }
    if(!strcmp(first,"type"))
    {
        return 2;
    }
    if(!strcmp(first,"printer"))
    {
        return 3;
    }
    if(!strcmp(first,"conversion"))
    {
        return 4;
    }
    if(!strcmp(first,"printers"))
    {
        return 5;
    }
    if(!strcmp(first,"jobs"))
    {
        return 6;
    }
    if(!strcmp(first,"print"))
    {
        return 7;
    }
    if(!strcmp(first,"cancel"))
    {
        return 8;
    }
    if(!strcmp(first,"pause"))
    {
        return 9;
    }
    if(!strcmp(first,"resume"))
    {
        return 10;
    }
    if(!strcmp(first,"disable"))
    {
        return 11;
    }
    if(!strcmp(first,"enable"))
    {
        return 12;
    }
    return -1;
    }
    return -1;
}