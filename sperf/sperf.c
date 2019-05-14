#include <unistd.h>
#include <stdio.h>
int main(int argc, char *argv[]) {
    /*int filedes[2];
    char buf[80];
    pid_t pid;
    
    pipe( filedes );
    pid=fork();        
    if (pid > 0){}
    else if(pid == 0){}*/
    printf("%s\n",argv);
    return 0;
}
