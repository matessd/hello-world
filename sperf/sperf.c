#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
double g_tot = 0;//总时间
typedef struct{
  char name[64];
  double time;
}Node;
Node a_list[256];
int list_cnt = 0;
time_t g_time = 0;
char buf[1024];

int cmp(const void*a, const void *b){
  double aa = ((Node*)a)->time;
  double bb = ((Node*)b)->time;
  if(bb>aa)
    return 1;
  else return -1;
}

void init(){
  list_cnt = 0;
  time(&g_time);
  g_tot = 0;
}

void output(){
    qsort(&a_list[0],list_cnt,sizeof(a_list[0]),cmp);
    printf("\033[2J");
    for(int i=0; i<list_cnt; i++){
      printf("%s:%.0lf%%\n",a_list[i].name,a_list[i].time/g_tot*100);
    }
}

void loop(){
  char name[128];
  int if_time_long=0;
  while(1){ 
    if(g_time+2<=time(0)){
      if_time_long=1;
      time(&g_time);
      output();
      init();
    }
    //scanf("%c",name);//把回车读掉 
    double dgt=1;
    int flg = 0， ret=0;
    ret = scanf("%[^(]",name);
    if(ret==EOF) break;
    ret = scanf("%*[^<]%*[^0-9]%lf%*[^>]",&dgt);
    if(ret==EOF) break;
    /*puts(buf);
    if(name[0]=='0'){
      if_time_long=1;
      puts(buf);
      break;
    }*/
    if(strcmp(name,"exit_group")==0)
      break;
    g_tot+=dgt;
    for(int i=0; i<list_cnt; i++){
      if(strcmp(a_list[i].name,name)==0){
        flg=1; a_list[i].time+=dgt;
        break;
      }
    }
    if(flg==0){
      list_cnt++;
      strcpy(a_list[list_cnt-1].name,name);
      a_list[list_cnt-1].time = dgt;
    }
    //printf("%s\n",name);
    //printf("%lf\n",tmp);
  }
  if(if_time_long==0)
    output();
}

int main(int argc, char *argv[]) {
  int filedes[2];
  if(pipe( filedes )!=0){
    printf("pipe create error\n");
    return 0;
  }
  pid_t pid;
  pid=fork();        
  assert(pid>=0);
  if (pid > 0){
    //father
    close(filedes[1]);
    dup2(filedes[0],0);
    init();
    loop();
  }
  else if(pid == 0){
    //child
    assert(argc>1);
    char *argv_send[128];
    argv_send[0] = "strace";
    argv_send[1] = "-T";
    for(int i=1; i<argc; i++){
      argv_send[i+1] = argv[i];
    }
    argv_send[argc+1] = NULL;

    //char *envp[] = {"PATH=/bin",NULL};

    int null_fd = open("/dev/null",O_RDWR);
    assert(null_fd>=0);
    dup2(filedes[1],2);
    dup2(null_fd,1);
    close(filedes[0]);
    execv("/usr/bin/strace",argv_send);
    //printf("execve error, should not reach here\n");
  }
  return 0;
}
