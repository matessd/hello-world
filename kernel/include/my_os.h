//#include<x86.h>
#include<am.h>
#include<common.h>
#include<klib.h>
#include<devices.h>
//#define MAX_CPU 8
//in x86-qemu.h
/*#define LOCKDEF(name) \
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
  }*/

//声明了NDEBUG后启动assert
#define NDEBUG

//alloc.c
typedef struct Node{
  struct Node* nxt, *prev;
  uintptr_t st,ed;//可分配区间
  uint32_t fence;
}alloc_node,*palloc_node;
#define FENCE 0xcccccccc
volatile palloc_node a_head;

//spin.c
typedef struct Cli_state{
  int ncli, intena;
}cli_sta;
extern volatile cli_sta cpu_cli[8];//要加volatile吗?

//extern spinlock_t *os_lk;
//extern spinlock_t *create_lk;

//trap,irq
typedef struct{
  int seq;
  int ev;
  handler_t handler;
}irq_handler;
extern volatile int n_handler;

//thread.c
extern spinlock_t *task_lk;
void idle();
extern volatile int Ntask;
extern volatile int ntask[8];
//void add_tail(task_t *task);//for save
//void add_head(task_t *task, int cnt);//for sem and create
//void del_head();//for switch
//task_t *Task_head[8];
#define MAX_TA 20
task_t *tasks[8][MAX_TA];
int Curr[8];
#define current tasks[_cpu()][Curr[_cpu()]]
//task_t *Current_task[8];
//#define current (Current_task[_cpu()])
//#define task_head (Task_head[_cpu()]) 
_Context *kmt_context_save(_Event ev, _Context *context);
_Context *kmt_context_switch(_Event ev, _Context *context);

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

