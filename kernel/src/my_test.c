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

int mygets(char *dst,const char *src){
  dst[0] = '\0';
  int i = 0;
  for(i=0; src[i]; i++){
    if(src[i]==' ') break;
    dst[i] = src[i];
  }
  dst[i] = '\0';
  return i;
}

void echo_task(void *name) {
  device_t *tty = dev_lookup(name);
  fs_t *fs = fs_list[0];
  inode_t *inode = &fs->inode_tab[0];
  while (1) {
    char line[1014], text[1024], cmd[32];
    sprintf(text, "(%s) $ ", name); 
    
    tty->ops->write(tty, 0, text, strlen(text));
    int nread = tty->ops->read(tty, 0, line, sizeof(line));
    line[nread - 1] = '\0';
    printf("%s**\n",line);

    char ctmp[128]; ctmp[0] = '\0';
    int cnt = mygets(cmd, line);
     
    if(strcmp(cmd,"ls")==0){
      sprintf(text, ".  ..");
      for(int i=2; i<MAX_DIR; i++){
        if(inode->child[i]){
          sprintf(ctmp, "  %s", inode->child[i]->name);
          strcat(text, ctmp);
        }
      }
      strcat(text, "\n");
    }else if(strcmp(cmd,"mkdir")==0){

    }
    else{
      text[0] = '\0';
    }
    //sprintf(text, "Echo: %s.\n", line); 
    tty->ops->write(tty, 0, text, strlen(text));
  }
}
