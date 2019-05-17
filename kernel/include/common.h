#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <nanos.h>

//my
#define STK_SZ 4096
struct task {
  int32_t id;
  char *name;
  _Context context;
  uint32_t fence1;
  char stack[STK_SZ];
  uint32_t fence2;
};
//spin.c
struct spinlock{
  const char *name;
  intptr_t locked, cpu;
};
struct semaphore {};

#endif
