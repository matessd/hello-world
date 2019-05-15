#include <common.h>
#include <klib.h>
#include <my_os.h>
static uintptr_t pm_start, pm_end;

//my
#define MIN_LEN 16
LOCKDEF(alloc)

/*typedef struct Node{
  struct Node* nxt, *prev;
  uintptr_t st,ed;//可分配区间
}alloc_node,*palloc_node;
volatile palloc_node a_head;*/

static void pmm_init() {
  pm_start = (uintptr_t)_heap.start;
  pm_end   = (uintptr_t)_heap.end;
  //my
  a_head = (palloc_node)pm_start;
  a_head->nxt = NULL;
  a_head->prev = NULL;
  a_head->st = ((uintptr_t)a_head)+MIN_LEN;
  a_head->ed = pm_end;//不可用
}

static void *kalloc(size_t size) {
  //my
  alloc_lock();
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
  alloc_unlock();
  return ret+1;
}

static void kfree(void *ptr) {
  //会不会free一个非链表元素？
  if(ptr==NULL) return;
  alloc_lock();
  palloc_node cur = (palloc_node)ptr;
  if(cur==a_head){
    cur->st = ((uintptr_t)cur)+MIN_LEN;
    cur->nxt = NULL;
    return;
  }
  palloc_node prev = cur->prev;
  palloc_node nxt = cur->nxt;
  //assert(prev!=NULL);
  prev->ed = cur->ed;
  prev->nxt = nxt;
  if(nxt!=NULL) nxt->prev = prev;
  alloc_unlock();
}

MODULE_DEF(pmm) {
  .init = pmm_init,
  .alloc = kalloc,
  .free = kfree,
};
