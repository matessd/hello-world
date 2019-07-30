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

void merge_path(char *dst, char *src, char *cur_dir){
  if(src[0]=='/') strcpy(dst, src); 
  else{
    sprintf(dst, "%s", cur_dir);
    if(strcmp(cur_dir,"/")!=0)
      strcat(dst,"/");
    strcat(dst, src);
  }
}

void get_dir_name(char *dst, inode_t *inode){
  char ctmp[128];
  dst[0] = '\0';
  if(strcmp(inode->name,"/")==0){
    strcpy(dst, "/"); return;
  }
  while(1){
    strcpy(ctmp, "/");
    if(strcmp(inode->name,"/")==0) return;
    strcpy(&ctmp[1],inode->name);
    strcat(ctmp,dst);
    strcpy(dst,ctmp);
    inode = inode->prev;
  }
}

void echo_task(void *name) {
  device_t *tty = dev_lookup(name);
  fs_t *fs = fs_list[0];
  inode_t *inode = &fs->inode_tab[0];
  char cur_dir[64]; 
  cur_dir[0] = '/'; cur_dir[1] = '\0'; 
  while (1) {
    char line[1014], text[1024];
    sprintf(text, "(%s):%s$ ", name, cur_dir); 

    tty->ops->write(tty, 0, text, strlen(text));
    int nread = tty->ops->read(tty, 0, line, sizeof(line));
    line[nread - 1] = '\0';

    char ctmp[128]; ctmp[0] = '\0';
    char cmd1[16], cmd2[DIR_NAME_LEN];
    char err[32];
    text[0] = '\0';
    int cur = 0;
    for(cur=0; line[cur]==' '; cur++);
    int cnt = mygets(cmd1, &line[cur]);
    cur+=cnt;
    cnt = mygets(cmd2, &line[cur]); 
    cur+=cnt;
    if(strcmp(cmd1,"cd")==0){
      if(cmd2[0]=='\0'){
        cur_dir[0] = '/'; cur_dir[1] = '\0'; 
        fs = fs_list[0];
        inode = &fs->inode_tab[0];
        continue;
      }
      merge_path(ctmp, cmd2, cur_dir);
      inode_t *p = vfs->find(ctmp);
      if(p!=NULL && p->sta==0){
        get_dir_name(ctmp, p);//deal .. and .
        strcpy(cur_dir, ctmp);
        inode = p;
        fs = inode->fs;
      }else{
        if(p==NULL)
          sprintf(err, "cd: No such dir\n");
        else
          sprintf(err, "cd: Not a dir, can't choose it\n");
        tty->ops->write(tty, 0, err, strlen(err));
        continue;
      }
    }else if(strcmp(cmd1,"ls")==0){
      sprintf(text, ".  ..");
      for(int i=0; i<MAX_DIR; i++){
        if(inode->child[i]){
          sprintf(ctmp, "  %s", inode->child[i]->name);
          strcat(text, ctmp);
          if(inode->child[i]->sta==0)
            strcat(text, "/");
        }
      }
      strcat(text, "\n");
    }else if(strcmp(cmd1,"mkdir")==0){
      if(cmd2[0]=='\0'){
        sprintf(err, "mkdir: Miss operand\n");
        tty->ops->write(tty, 0, err, strlen(err));
        continue;
      }
      merge_path(ctmp, cmd2, cur_dir);
      //printf("%s\n",ctmp);
      int ret = vfs->mkdir(ctmp, 0, 0);
      if(ret==1){
        sprintf(err, "mkdir: Already exist\n");
        tty->ops->write(tty, 0, err, strlen(err));
        continue;
      }else if(ret==2){
        sprintf(err, "mkdir: No such directory\n");
        tty->ops->write(tty, 0, err, strlen(err));
        continue;
      }else if(ret==3){
        sprintf(err, "mkdir: No permission\n");
        tty->ops->write(tty, 0, err, strlen(err));
        continue;
      }
    }else if(strcmp(cmd1, "rm")==0){
      if(cmd2[0]=='\0'){
        sprintf(err, "rm: Miss operand\n");
        tty->ops->write(tty, 0, err, strlen(err));
        continue;
      }
      char cmd3[70];
      if(strcmp(cmd2,"-r")==0){
        mygets(cmd3, &line[cur]);
        if(cmd3[0]=='\0'){
          sprintf(err, "rm: Miss operand\n");
          tty->ops->write(tty, 0, err, strlen(err));
          continue;
        }
        merge_path(ctmp, cmd3, cur_dir);
      }else{
        merge_path(ctmp, cmd2, cur_dir);
        inode_t *p = vfs->find(ctmp);
        //printf("%s**%s\n",p->name,ctmp);
        if(p!=NULL){
          //assert(0);
          if(p->sta==0){
            sprintf(err, "rm: Can't remove, it's a directory\n");
            tty->ops->write(tty, 0, err, strlen(err));
            continue;
          }
        }
      }
      int ret = vfs->rmdir(ctmp, 0);
      if(ret==2){
        sprintf(err, "rm: No permission\n");
        tty->ops->write(tty, 0, err, strlen(err));
        continue;
      }else if(ret==1){
        sprintf(err, "rm: No such dir or file\n");
        tty->ops->write(tty, 0, err, strlen(err));
        continue;
      }
    }else if(strcmp(cmd1, "cat")==0){
      if(cmd2[0]=='\0'){
        sprintf(err, "cat: Miss operand\n");
        tty->ops->write(tty, 0, err, strlen(err));
        continue;
      }
      merge_path(ctmp, cmd2, cur_dir);
      int ret = vfs->read(ctmp, text, 0);
      if(ret==-1){
        sprintf(err, "cat: No such file\n");
        tty->ops->write(tty, 0, err, strlen(err));
        continue;
      }else if(ret==-2){
        sprintf(err, "cat: Not a file\n");
        tty->ops->write(tty, 0, err, strlen(err));
        continue;
      }
    }else if(strcmp(cmd1, "touch")==0){
      if(cmd2[0]=='\0'){
        sprintf(err, "touch: Miss operand\n");
        tty->ops->write(tty, 0, err, strlen(err));
        continue;
      }
      merge_path(ctmp, cmd2, cur_dir);
      int ret = vfs->mkdir(ctmp, 1, 0);
      if(ret==1){
        sprintf(err, "mkdir: Already exist\n");
        tty->ops->write(tty, 0, err, strlen(err));
        continue;
      }else if(ret==2){
        sprintf(err, "mkdir: No such directory\n");
        tty->ops->write(tty, 0, err, strlen(err));
        continue;
      }else if(ret==3){
        sprintf(err, "mkdir: No permission\n");
        tty->ops->write(tty, 0, err, strlen(err));
        continue;
      }
    }
    else{
      sprintf(err, "No such cmd\n");
      tty->ops->write(tty, 0, err, strlen(err));
      continue;
    }
    tty->ops->write(tty, 0, text, strlen(text));
  }
}
