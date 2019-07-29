#include<my_os.h>

void inode_init(inode_t *inode, int32_t blkno, int inodeno, int8_t sta, const char *name, fs_t *fs, inode_t *prev, int8_t lmt){
  inode->blkno = inodeno;
  inode->inodeno = inodeno;
  inode->sta = sta;
  strcpy(inode->name, name);
  inode->fs = fs;
  inode->prev = prev;
  inode->lmt = lmt;
}

void proc_init(){
  char ctmp[128]; ctmp[0] = '\0';
  char src[128];
  for(int i=0; i<_ncpu(); i++){
    sprintf(src,"processor: %d\n",i);
    strcat(ctmp, src);
    sprintf(src,"cpu cores: xxxx\n");
    strcat(ctmp, src);
  }
  vfs->write("/proc/cpuinfo", ctmp, 0);

  sprintf(src,"MemTotal: xxx kB\n");
  strcpy(ctmp, src);
  sprintf(src,"MemFree: xxx kB\n");
  strcat(ctmp, src);
  vfs->write("/proc/meminfo", ctmp, 0);
}

void vfs_init(){
  fs_t *fs = pmm->alloc(sizeof(fs_t));
  fs_list[0] = fs;
  fs->inode_map[0] = 1;
  inode_t *inode = &fs->inode_tab[0];
  inode_init(inode, (int32_t)-1, 0, 0, "/", fs, inode, 0);
  vfs->mkdir("/mnt", 0, 0);
  vfs->mkdir("/dev", 0, 1);
  vfs->mkdir("/proc", 0, 1);
  vfs->mkdir("/dev/tty1", 3, 1);
  vfs->mkdir("/dev/tty2", 3, 1);
  vfs->mkdir("/dev/tty3", 3, 1);
  vfs->mkdir("/dev/tty4", 3, 1);
  vfs->mkdir("/dev/ramdisk0", 2, 1);
  vfs->mkdir("/dev/ramdisk1", 2, 1);

  vfs->mkdir("/proc/cpuinfo", 1, 1);
  vfs->mkdir("/proc/meminfo", 1, 1);
  proc_init();
}

int valid_inode(fs_t *fs){
  for(int i=0; i<MAX_inode; i++){
    if(fs->inode_map[i]==0) return i;
  }
  return -1;
}

int valid_blk(fs_t *fs){
  for(int i=0; i<MAX_blk; i++){
    if(fs->blk_map[i]==0) return i;
  }
  return -1;
}

inode_t *find_inode(const char *path){
  //assert(path[0]=='/');

  //init
  fs_t *ram = fs_list[0];
  inode_t *inode = &ram->inode_tab[0];
  inode_t *child = NULL;
  char ctmp[128]; int cur=0;
  ctmp[0] = '/'; ctmp[1] = '\0';
  int flg = 0;

  //success or fail
  if(strcmp(path,"/")==0) return inode;
  for(int i=1; path[i]; i++){
    if(path[i]=='/' || path[i+1]=='\0'){
      if(path[i+1]=='\0' && path[i]!='/'){
        ctmp[cur++] = path[i];
        ctmp[cur] = '\0';
      }
      cur = 0; flg = 0;
      if(strcmp(ctmp,".")==0) {
        if(path[i+1]=='\0') return inode;
        continue;
      }
      if(strcmp(ctmp,"..")==0){
        inode = inode->prev;
        ram = inode->fs;
        if(path[i+1]=='\0') return inode;
        continue;
      }
      for(int j=0; j<MAX_DIR; j++){
        child = inode->child[j];
        if(child){
          if(strcmp(child->name, ctmp)==0){
            if(child->sta==0 || path[i+1]=='\0'){
              inode = child; 
              ram = inode->fs;            
              flg = 1;
              break;
            }
          } 
        }
      }
      if(flg==0) return NULL;//no such file or dir
      if(path[i+1]=='\0') return inode;
    }else{
      ctmp[cur++] = path[i];
      ctmp[cur] = '\0';
    }
  }
  //assert(0); //can't reach here
  return NULL;
}

int vfs_mkdir(const char *path1, int8_t sta, int8_t lmt){
  //assert(path[0]=='/');

  //init
  fs_t *ram = fs_list[0];
  inode_t *inode = &ram->inode_tab[0];
  inode_t *child = NULL;
  char ctmp[128], path[128]; int cur=0;
  strcpy(path, path1);
  ctmp[0] = '/'; ctmp[1] = '\0';
  int flg = 0;

  //success or fail
  if(strcmp(path,"/")==0) return 1;//already exists
  for(int i=1; path[i]; i++){
    if(path[i]=='/' || path[i+1]=='\0'){
      if(strcmp(path,"/proc/1")==0)
        printf("%s&&%s\n",ctmp,&path[i]);
      if(path[i+1]=='\0' && path[i]!='/'){
        ctmp[cur++] = path[i];
        ctmp[cur] = '\0';
      }
      if(strcmp(path,"/proc/1")==0)
        printf("%s**%d\n",ctmp,i);
      cur = 0; flg = 0;
      if(strcmp(ctmp,".")==0) {
        if(path[i+1]=='\0') return 1;
        continue;
      }
      if(strcmp(ctmp,"..")==0){
        if(path[i+1]=='\0') return 1;
        inode = inode->prev;
        ram = inode->fs;
        continue;
      }
      for(int j=0; j<MAX_DIR; j++){
        child = inode->child[j];
        if(child){
          if(strcmp(child->name, ctmp)==0){
            if(child->sta==0 || path[i+1]=='\0'){
              inode = child; 
              ram = inode->fs;            
              flg = 1;
              break;
            }
          } 
        }
      }
      if(flg==0 && path[i+1]!='\0'){
        return 2;//no such dir
      }
      if(path[i+1]=='\0' && flg==1){
        return 1;//already exist
      }
    }else{
      ctmp[cur++] = path[i];
      ctmp[cur] = '\0';
    }
  }

  //success, now create new dir
  if(lmt<inode->lmt) return 3;//no permission
  int inodeno = valid_inode(ram);
  //assert(inodeno>=0);
  ram->inode_map[inodeno] = 1;
  child = &ram->inode_tab[inodeno];
  if(sta==0)
    inode_init(child, -1, inodeno, sta, ctmp, ram, inode, lmt);
  else{
    int blkno = valid_blk(ram);
    //assert(blk>=0);
    ram->blk_map[blkno] = 1;
    inode_init(child, blkno, inodeno, sta, ctmp, ram, inode, lmt);
  }
  for(int i=0; i<MAX_DIR; i++){
    if(inode->child[i]==NULL){
      inode->child[i] = child;
      break;
    }
  }
  return 0;
}

void del_inode(inode_t *inode){
  for(int i=0; i<MAX_DIR; i++){
    if(inode->child[i]){
      del_inode(inode->child[i]);
    }
  }
  int inodeno = inode->inodeno, blkno = inode->blkno;
  if(blkno!=-1)
    inode->fs->blk_map[blkno] = 0;
  inode->fs->inode_map[inodeno] = 0;
  inode_t *prev = inode->prev;
  for(int i=0; i<MAX_DIR; i++){
    if(prev->child[i]==inode){
      prev->child[i] = NULL;
    }
  }
  memset(inode, 0, sizeof(*inode));
}

int vfs_rmdir(const char *path, int8_t lmt){
  //assert(path!=NULL);
  //assert(path[0]=='/');
  if(strcmp(path, "/")==0) return 2;
  inode_t *inode = vfs->find(path);
  if(inode==NULL)
    return 1;
  if(lmt<inode->lmt) return 2;
  del_inode(inode);
  return 0;
}

ssize_t vfs_read (const char *path, void *buf, int off){
  inode_t *inode = vfs->find(path);
  if(inode==NULL) return -1;
  int sta = inode->sta;
  if(sta!=1) return -2;
  int blkno = inode->blkno;
  char *dst = (char *)buf;
  dst[0] = '\0';
  strcpy(dst, &inode->fs->blk[blkno][off]);
  return 0;
}

ssize_t vfs_write(const char *path, void *buf, int off){
  //printf("%s ** %s\n",path, (char*)buf);
  inode_t *inode = vfs->find(path);
  if(inode==NULL) return -1;
  int sta = inode->sta;
  if(sta!=1) return -2;
  int blkno = inode->blkno;
  inode->fs->blk[blkno][off] = '\0';
  strcpy(&inode->fs->blk[blkno][off], (char *)buf);
  return 0;
}

MODULE_DEF(vfs) {
  .init = vfs_init,
    .mkdir = vfs_mkdir,
    .find = find_inode,
    .rmdir = vfs_rmdir,
    .read = vfs_read,
    .write = vfs_write,
};

