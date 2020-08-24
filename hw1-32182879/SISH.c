#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<time.h>
#include<stdlib.h>
#define MAX 255
void *prompt(char cBuf[]){
time_t rawtime;
struct tm *tmi;
char hBuf[MAX], uBuf[MAX], dBuf[MAX];
char *now;
void *ret;
time(&rawtime);//passed time
tmi = localtime(&rawtime);//translate rawtime
now = asctime(tmi);//make sturct to string
now[strlen(now)-1] = 0;//add null
gethostname(hBuf,MAX);//save hostname
getlogin_r(uBuf,MAX);//save username
getcwd(dBuf, MAX);//save directoryname
printf("[%s]%s@%s%s$",now,hBuf,uBuf,dBuf);
ret = fgets(cBuf, MAX, stdin);

if(cBuf[strlen(cBuf)-1] == '\n')cBuf[strlen(cBuf)-1] = 0;
return ret;}

void zombie_handler(int signo)
{
    pid_t pid ;
    int stat ;
    
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated normaly\n", pid) ;
}

int main()
{ char cBuf[MAX];
char buf[MAX];
char *arg;
pid_t pid;
int status;
while(prompt(cBuf)){
  if(!strncmp(cBuf,"exit",strlen(cBuf)))break;
  else if((pid = fork()) < 0)perror("fork error");
  else if(pid == 0){
          strtok(cBuf," ");
          arg = strtok(NULL, " ");
          if(arg == NULL) execlp(cBuf,cBuf,(char*)0);//no argument
          else {//one argument
               if(strcmp(cBuf,"cd")==0)
               {chdir(arg); _exit(0);}
               else if(strcmp(cBuf,"pwd")==0)
               {getcwd(buf,MAX);printf("directory: %s\n",buf);_exit(0);}
               /*else if(strcmp(cBuf,"zombie_handler")==0)
               {zombie_handler(getpid(arg));_exit(0);}*/
               else execlp(cBuf,cBuf,arg,(char*)0);}
          perror("couldn't execute");}
  waitpid(pid, &status, 0);}
 exit(0);
}




