#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
double g_tot = 0;//总时间
typedef struct{
    char name[64];
    double time;
}Node;
Node a_list[256];
int list_cnt = 0;
char buf[1024];

int cmp(const void*a, const void *b){
    int aa = ((Node*)a)->time;
    int bb = ((Node*)b)->time;
    if(bb>aa)
        return 1;
    else return -1;
}

void readin(){
    char name[128];
    while(scanf("%[^\n]%*c",buf)!=EOF){ 
        //scanf("%c",name);//把回车读掉 
        double dgt=1;
        int flg = 0;
        sscanf(buf,"%[^(]",name);
        sscanf(buf,"%*[^<]%*[^0-9]%lf%*[^>]",&dgt);
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
        readin();
        qsort(&a_list[0],list_cnt,sizeof(a_list[0]),cmp);
        for(int i=0; i<list_cnt; i++){
            printf("%s:%.0lf%%\n",a_list[i].name,a_list[i].time/g_tot*100);
        }
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
        
        char *envp[] = {"PATH=/bin",NULL};

        int null_fd = open("/dev/null",O_RDWR);
        assert(null_fd>=0);
        dup2(filedes[1],2);
        dup2(null_fd,1);
        close(filedes[0]);
        execve("/usr/bin/strace",argv_send,envp);
        //printf("execve error, should not reach here\n");
    }
    return 0;
}
