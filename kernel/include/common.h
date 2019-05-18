#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <nanos.h>

//my
#define STK_SZ 4096
struct task {
  _Context context;
  int32_t id, sleep_flg;
  const char *name;
  struct task *nxt;
  uint32_t fence;
  char stack[STK_SZ];
  //uint32_t fence2;
};
//spin.c
struct spinlock{
  const char *name;
  intptr_t locked, cpu;
};
//NPROC定多大比较好？
#define NPROC 100
struct semaphore {  
  volatile int value;
  spinlock_t lk;
  const char* name;
  task_t *queue[NPROC];
  int end;
  int start;
};

#endif
