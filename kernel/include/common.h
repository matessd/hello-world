#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <nanos.h>

struct task {};
//spin.c
struct spinlock{
  char *name;
  uint32_t lock_ed, cpu;
};
struct semaphore {};

#endif
