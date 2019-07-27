#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <am.h>
#include <nanos.h>

#define MODULE(name) \
  mod_##name##_t; \
  extern mod_##name##_t *name
#define MODULE_DEF(name) \
  extern mod_##name##_t __##name##_obj; \
  mod_##name##_t *name = &__##name##_obj; \
  mod_##name##_t __##name##_obj = 

typedef _Context *(*handler_t)(_Event, _Context *);
//os
typedef struct {
  void (*init)();
  void (*run)();
  _Context *(*trap)(_Event ev, _Context *context);
  void (*on_irq)(int seq, int event, handler_t handler);
} MODULE(os);

//pmm
typedef struct {
  void (*init)();
  void *(*alloc)(size_t size);
  void (*free)(void *ptr);
} MODULE(pmm);

typedef struct task task_t;
typedef struct spinlock spinlock_t;
typedef struct semaphore sem_t;

//kmt
typedef struct {
  void (*init)();
  int (*create)(task_t *task, const char *name, void (*entry)(void *arg), void *arg);
  void (*teardown)(task_t *task);
  void (*spin_init)(spinlock_t *lk, const char *name);
  void (*spin_lock)(spinlock_t *lk);
  void (*spin_unlock)(spinlock_t *lk);
  void (*sem_init)(sem_t *sem, const char *name, int value);
  void (*sem_wait)(sem_t *sem);
  void (*sem_signal)(sem_t *sem);
} MODULE(kmt);

//device
typedef struct device device_t;
typedef struct devops {
  int (*init)(device_t *dev);
  ssize_t (*read)(device_t *dev, off_t offset, void *buf, size_t count);
  ssize_t (*write)(device_t *dev, off_t offset, const void *buf, size_t count);
} devops_t;

typedef struct {
  void (*init)();
} MODULE(dev);

//filesystem
typedef struct fsops{
  //void (*init)(fs_t *fs, const char *name, device_t *dev);
  //inode_t *(*lookup)(fs_t *fs, const char *path, int flags);
  //int (*close)(inode_t *inode);
} fsops_t;

typedef struct fs fs_t;
typedef struct inode inode_t;

#define MAX_inode 1024
struct fs{
  int8_t inode_map[MAX_inode];
  int8_t blk_map[1024];
  inode_t inode_tab[MAX_inode];
  char blk[1024][512];
  fsops_t *ops;
  device_t *dev;
};

#define MAX_DIR 64
#define DIR_NAME_LEN 64
struct fs;
struct inode{
  int32_t blkno;
  //0 is dir, 1 is fs(block dev), 2 is file, 3 is dev
  int8_t sta;
  char name[DIR_NAME_LEN];
  fs_t *fs;
  inode_t *prev;//上一级目录名
  //int cnt;//subdir NO
  inode_t *child[MAX_DIR];
};

typedef struct{
  void (*init)();
  //int (*access)(const char *path, int mode);
  //int (*mount)(const char *path, fs_t *fs);
  //int (*unmount)(const char *path);
  int (*mkdir)(const char *path);
  int (*rmdir)(const char *path);
  //int (*link)(const char *oldpath, const char *newpath);
  //int (*unlink)(const char *path);
  int (*open)(const char *path, int flags);
  ssize_t (*read)(int fd, void *buf, size_t nbyte);
  ssize_t (*write)(int fd, void *buf, size_t nbyte);
  //off_t (*lseek)(int fd, off_t offset, int whence);
  int (*close)(int fd);
} MODULE(vfs);

#endif
