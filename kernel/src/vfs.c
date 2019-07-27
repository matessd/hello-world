#include<my_os.h>
fs_t *fs_list[16];

void inode_init(inode_t *inode, int32_t no, int8_t sta, const char *name, fs_t *fs, inode_t *prev){
  inode->blkno = no;
  inode->sta = sta;
  strcpy(inode->name, name);
  inode->fs = fs;
  inode->prev = prev;
}

void vfs_init(){
  fs_t *fs = pmm->alloc(sizeof(fs_t));
  fs_list[0] = fs;
  fs->inode_map[0] = 1;
  inode_t *inode = &fs->inode_tab[0];
  inode_init(inode, (int32_t)-1, 0, "/", fs, NULL);
  //sprintf(ram0->inode_tab[0].name, "/");
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
  inode_t *child = NULL, *prev = NULL;
  char ctmp[128]; int cur=0;
  ctmp[0] = '/'; ctmp[1] = '\0';
  int flg = 0;

  //success or fail
  for(int i=0; path[i]; i++){
    if(path[i]=='/' || path[i+1]=='\0'){
      cur = 0; flg = 0;
      for(int j=0; j<MAX_DIR; j++){
        child = inode->child[j];
        if(child){
          if(strcmp(child->name, ctmp)==0 && child->sta==0){
            prev = inode;
            inode = child; 
            ram = inode->fs;            
            flg = 1;
            break;
          } 
        }
      }
      if(flg==0 && path[i+1]!='\0'){
        return 2;//no such dir
      }
      if(path[i+1]=='\0'){
        if(strcmp(inode->name, ctmp)==0)return 1;//already exist
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

