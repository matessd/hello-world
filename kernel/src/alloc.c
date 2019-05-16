#include <klib.h>
#include <my_os.h>
static uintptr_t pm_start, pm_end;

//my
#define MIN_LEN 20
#define INIT_VALUE 0xcccccccc
/*LOCKDEF(alloc)
spinlock_t Os_lk;
spinlock_t *os_lk;

typedef struct Node{
  struct Node* nxt, *prev;
  uintptr_t st,ed;//可分配区间
}alloc_node,*palloc_node;
volatile palloc_node a_head;*/

spinlock_t Alloc_lk;
spinlock_t *alloc_lk=&Alloc_lk;
cli_sta cpu_cli[8];

static void pmm_init() {
  pm_start = (uintptr_t)_heap.start;
  pm_end   = (uintptr_t)_heap.end;
  //my
  a_head = (palloc_node)pm_start;
  a_head->nxt = NULL;
  a_head->prev = NULL;
  a_head->st = ((uintptr_t)a_head)+MIN_LEN;
  a_head->ed = pm_end;//不可用
  a_head->fence = INIT_VALUE;
  //my spin_lock
  for(int i=0; i<8; i++)
    cpu_cli[i].ncli = 0;
  kmt->spin_init(os_lk, "os_lk");
  kmt->spin_init(alloc_lk, "alloc_lk");
}

static void *kalloc(size_t size) {
  //my
  //printf("%d\n",size);
  kmt->spin_lock(alloc_lk);
  //assert(size>=0);
  if(size==0) return NULL;
  palloc_node ret = NULL;
  palloc_node cur = a_head;
  size_t new_size = size+MIN_LEN;
  while(cur!=NULL){
    //assert(a_head->ed>=a_head->st);
    if(cur->ed - cur->st >=new_size){
      ret = (palloc_node)(cur->ed - new_size);
      ret->st = ret->ed = cur->ed;
      cur->ed = (uintptr_t)ret;

      palloc_node nxt = cur->nxt;
      ret->nxt = nxt;
      if(nxt!=NULL) nxt->prev = ret;

      ret->prev = cur;
      cur->nxt = ret;
      break;
    }
    cur = cur->nxt;
  }
  if(ret==NULL) return NULL;

  //清零，为了tty
  memset(ret+1,0,size);

  ret->fence = INIT_VALUE;
  kmt->spin_unlock(alloc_lk);
  return ret+1;
}

static void kfree(void *ptr) {
  if(ptr==NULL) return;
  kmt->spin_lock(alloc_lk);
  palloc_node cur = ((palloc_node)ptr)-1;
  assert(cur->fence == INIT_VALUE);//保证free有效性
  cur->fence = 0;
  if(cur==a_head){
    cur->st = ((uintptr_t)cur)+MIN_LEN;
    return;
  }
  palloc_node prev = cur->prev;
  palloc_node nxt = cur->nxt;
  //assert(prev!=NULL);
  prev->ed = cur->ed;
  prev->nxt = nxt;
  if(nxt!=NULL) nxt->prev = prev;
  kmt->spin_unlock(alloc_lk);
}

MODULE_DEF(pmm) {
  .init = pmm_init,
  .alloc = kalloc,
  .free = kfree,
};
