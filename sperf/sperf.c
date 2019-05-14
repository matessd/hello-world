#include <unistd.h>
#include <stdio.h>
#include <assert.h>
int main(int argc, char *argv[]) {
    int filedes[2];
    char buf[80];
    pid_t pid;
    
    pipe( filedes );
    pid=fork();        
    if (pid > 0){
        
    }
    else if(pid == 0){
        assert(argc>1);
        char *argv_send[] = argv[1];
        execve("/bin/strace","strace","-T", argv_send, argv_send,NULL);
    }
    //printf("%s\n",argv[1]);
    return 0;
}
