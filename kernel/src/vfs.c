#include<my_os.h>
typedef struct{
  int8_t inode_map;
  int8_t blk_map;
  char inode_table;
  char data_blk;
  fsops_t *pos;
  device_t *dev;
}fs_t;

typedef struct fsops{
  void (*init)(fs_t *fs, const char *name, device_t *dev);
  //inode_t *(*lookup)(fs_t *fs, const char *path, int flags);
  //int (*close)(inode_t *inode);
} fsops_t;

void vfs_init(){

}


MODULE_DEF(vfs) {
  .init = vfs_init,
};

