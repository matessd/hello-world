#include<my_os.h>

void vfs_init(){
  fs_t *ram = kalloc(sizeof(fs_t));
  int len = sizeof(fs_t);
  printf("%x\n",len);
}


MODULE_DEF(vfs) {
  .init = vfs_init,
};

