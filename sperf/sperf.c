#include <unistd.h>
#include <stdio.h>
#include <assert.h>
int main(int argc, char *argv[]) {
    /*int filedes[2];
    char buf[80];
    
    pipe( filedes );*/
    pid_t pid;
    pid=fork();        
    if (pid > 0){
        
    }
    else if(pid == 0){
        assert(argc>1);
        char *argv_send[128];
        argv_send[0] = "strace";
        argv_send[1] = "-T";
        for(int i=1; i<argc; i++){
            argv_send[i+1] = argv[i];
        }
        argv_send[argc+1] = NULL;
        printf("%s\n",argv_send[2]);
        execve("/bin/strace",argv_send,NULL);
    }
    //printf("%s\n",argv[1]);
    return 0;
}
