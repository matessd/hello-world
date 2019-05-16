#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <nanos.h>

struct task {};
//spin.c
struct spinlock{
  const char *name;
  uint32_t locked, cpu;
};
struct semaphore {};

#endif
