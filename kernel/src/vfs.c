#include<my_os.h>

void vfs_init(){

}


MODULE_DEF(vfs) {
  .init = vfs_init,
};

