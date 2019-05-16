#include<x86.h>
#include<am.h>
#define MAX_CPU 8
//in x86-qemu.h
#define LOCKDEF(name) \
  static volatile intptr_t name##_locked = 0; \
  static int name##_lock_flags[MAX_CPU]; \
  void name##_lock() { \
    name##_lock_flags[_cpu()] = get_efl() & FL_IF; \
    cli(); \
    while (1) { \
      if (0 == _atomic_xchg(&name##_locked, 1)) break; \
      pause(); \
    } \
  } \
  void name##_unlock() { \
    _atomic_xchg(&name##_locked, 0); \
    if (name##_lock_flags[_cpu()]) sti(); \
  }

//声明了NDEBUG后启动assert
#define NDEBUG

//alloc.c
typedef struct Node{
  struct Node* nxt, *prev;
  uintptr_t st,ed;//可分配区间
  uint32_t fence;
}alloc_node,*palloc_node;
volatile palloc_node a_head;

//spin.c
typedef struct Cli_state{
  int ncli, intena;
}cli_sta;
cli_sta cpu_cli[8];
spinlock_t Os_lk;
spinlock_t *os_lk=&Os_lk;

//for debug
static inline void dputs(const char *s) {
  for (; *s; s++) {
    _putc(*s);
  }
}
#define STRINGIFY(s) #s
#define TOSTRING(s) STRINGIFY(s)
#define dpanic(s) \
  do { \
    dputs("AM dPanic: "); dputs(s); \
    dputs(" @ " __FILE__ ":" TOSTRING(__LINE__) "  \n"); \
    _halt(1); \
  } while(0)

