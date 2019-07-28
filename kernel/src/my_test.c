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
  int i = 0, flg = 0, cur=0;
  for(i=0; src[i]; i++){
    if(flg && src[i]!=' ') break;
    if(src[i]==' '){
      flg = 1; continue;
    }
    dst[cur++] = src[i];
  }
  dst[cur] = '\0';
  return i;
}

void echo_task(void *name) {
  device_t *tty = dev_lookup(name);
  fs_t *fs = fs_list[0];
  inode_t *inode = &fs->inode_tab[0];
  while (1) {
    char line[1014], text[1024];
    sprintf(text, "(%s) $ ", name); 
    
    tty->ops->write(tty, 0, text, strlen(text));
    int nread = tty->ops->read(tty, 0, line, sizeof(line));
    line[nread - 1] = '\0';

    char ctmp[128]; ctmp[0] = '\0';
    char cmd1[16], cmd2[DIR_NAME_LEN];
    text[0] = '\0';
    int cur = 0, cnt = mygets(cmd1, line);
    cur+=cnt;
    mygets(cmd2, &line[cur]); 
    if(strcmp(cmd1,"ls")==0){
      sprintf(text, ".  ..");
      for(int i=2; i<MAX_DIR; i++){
        if(inode->child[i]){
          sprintf(ctmp, "  %s", inode->child[i]->name);
          strcat(text, ctmp);
        }
      }
      strcat(text, "\n");
    }else if(strcmp(cmd1,"mkdir")==0){
      char err[32];
      if(cmd2[0]=='\0'){
        sprintf(err, "mkdir: Miss operand\n");
        tty->ops->write(tty, 0, err, strlen(err));
        continue;
      }
      if(cmd2[0]=='/') strcpy(ctmp, cmd2); 
      int ret = vfs->mkdir(ctmp);
      if(ret==1){
        sprintf(err, "mkdir: Already exist\n");
        tty->ops->write(tty, 0, err, strlen(err));
      }else if(ret==2){
        sprintf(err, "mkdir: No such directory\n");
        tty->ops->write(tty, 0, err, strlen(err));
      }
    }
    else{
       
    }
    tty->ops->write(tty, 0, text, strlen(text));
  }
}
