#include<my_os.h>

void inode_init(inode_t *inode, int32_t no, int8_t sta, const char *name, fs_t *fs, inode_t *prev){
  inode->blkno = no;
  inode->sta = sta;
  strcpy(inode->name, name);
  inode->fs = fs;
  inode->prev = prev;
  inode->child[0] = inode;
  inode->child[1] = prev;
}

void vfs_init(){
  fs_t *fs = pmm->alloc(sizeof(fs_t));
  fs_list[0] = fs;
  fs->inode_map[0] = 1;
  inode_t *inode = &fs->inode_tab[0];
  inode_init(inode, (int32_t)-1, 0, "/", fs, inode);
  vfs->mkdir("/dev");
  vfs->mkdir("/proc");
  vfs->mkdir("/dev/tty1");
  vfs->mkdir("/dev/tty2");
  vfs->mkdir("/dev/tty3");
  vfs->mkdir("/dev/tty4");
}

int valid_inode(fs_t *fs){
  for(int i=0; i<MAX_inode; i++){
    if(fs->inode_map[i]==0) return i;
  }
  return -1;
}

int vfs_mkdir(const char *path){
  //assert(path[0]=='/');
  
  //init
  fs_t *ram = fs_list[0];
  inode_t *inode = &ram->inode_tab[0];
  inode_t *child = NULL, *prev = inode;
  char ctmp[128]; int cur=0;
  ctmp[0] = '/'; ctmp[1] = '\0';
  int flg = 0;

  //success or fail
  if(strcmp(path,"/")==0) return 1;//already exists
  for(int i=1; path[i]; i++){
    if(path[i]=='/' || path[i+1]=='\0'){
      if(path[i+1]=='\0' && path[i]!='/'){
        ctmp[cur++] = path[i];
        ctmp[cur] = '\0';
      }
      if(strcmp(ctmp,".")==0) {
        if(path[i+1]=='\0') return 1;
        continue;
      }
      if(strcmp(ctmp,"..")==0){
        if(path[i+1]=='\0') return 1;
        inode = inode->prev;
        prev = inode;
        ram = inode->fs;
        continue;
      }
      //printf("%s\n",ctmp);
      cur = 0; flg = 0;
      for(int j=2; j<MAX_DIR; j++){
        child = inode->child[j];
        if(child){
          if(strcmp(child->name, ctmp)==0){
            if(child->sta==0 || path[i+1]=='\0'){
              prev = inode;
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
  int inodeno = valid_inode(ram);
  assert(inodeno>0);
  ram->inode_map[inodeno] = 1;
  child = &ram->inode_tab[inodeno];
  inode_init(child, -1, 0, ctmp, ram, inode);
  for(int i=0; i<MAX_DIR; i++){
    if(inode->child[i]==NULL){
      inode->child[i] = child;
      break;
    }
  }
  return 0;
}

MODULE_DEF(vfs) {
  .init = vfs_init,
    .mkdir = vfs_mkdir,
};

