#include <unistd.h>
#include <stdio.h>
#include <assert.h>
int main(int argc, char *argv[]) {
    /*int filedes[2];
    char buf[80];
    
    pipe( filedes );*/
    pid_t pid;
    pid=fork();        
    assert(pid>=0);
    if (pid > 0){
        //father
        
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
        //printf("%s\n",argv_send[0]);
        int ret =execve("/usr/bin/strace",argv_send,envp);
        assert(ret!=-1);
        printf("11****\n");
    }
    return 0;
}
