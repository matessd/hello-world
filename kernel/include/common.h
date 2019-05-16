#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <nanos.h>

struct task {};
//spin.c
struct spinlock{
  const char *name;
  intptr_t locked, cpu;
};
struct semaphore {};

#endif
