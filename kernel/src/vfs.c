#include<my_os.h>

void vfs_init(){
  fs_t *ram = pmm->alloc(sizeof(fs_t));
  ram->blk_map[0] = 0;
  int len = sizeof(fs_t);
  printf("%d\n",len);
}


MODULE_DEF(vfs) {
  .init = vfs_init,
};

