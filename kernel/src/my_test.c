#include<my_os.h>
//char *ptr[8][100];
/*void my_pmmtest(){
  //char s[100];
  int cnt = 0;
  while(1){
    char *ptr = pmm->alloc(100);
    sprintf(ptr,"cpu:%d | cnt: %d\n",_cpu(),cnt);
    cnt++;
    printf("%s\n",ptr);
    pmm->free(ptr);
  }
}*/

int mygets(char *buf){
  
}

void echo_task(void *name) {
  device_t *tty = dev_lookup(name);
  fs_t *fs = fs_list[0];
  while (1) {
    char line[1014], text[1024], cmd[16];
    sprintf(text, "(%s) $ ", name); 
    
    tty->ops->write(tty, 0, text, strlen(text));
    int nread = tty->ops->read(tty, 0, line, sizeof(line));
    line[nread - 1] = '\0';
    
    mygets()
    if(strcmp())
    sprintf(text, "Echo: %s.\n", line); 
    tty->ops->write(tty, 0, text, strlen(text));
  }
}
