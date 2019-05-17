#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <nanos.h>

//my
#define STK_SZ 4096
struct task {
  _Context context;
  int32_t id;
  char *name;
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
struct semaphore {};

#endif
