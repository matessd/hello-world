#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
//#include <sys/types.h>
//#include <sys/stat.h>
double g_tot = 0;//总时间
typedef struct{
    char name[64];
    double time;
}Node;
Node a_list[256];
char buf[10];
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
        close(filedes[0]);
        read(filedes[0], buf, sizeof(buf));
        printf("%s>>>>>>>>\n",buf);
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

        int null_fd = open("/dev/null",O_WRONLY);
        assert(null_fd>=0);
        assert(dup2(null_fd,1)==1);//stdout
        //dup2(1, filedes[1]);
        execve("/usr/bin/strace",argv_send,envp);
        printf("execve error, should not reach here\n");
    }
    return 0;
}
