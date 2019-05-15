#include <common.h>
#include <klib.h>
#include <x86.h>

static uintptr_t pm_start, pm_end;

//my
#define MIN_LEN 12
#define NEXT(x) ((palloc_node)((uintptr_t)x+x->size))
LOCKDEF(alloc)

typedef struct Node{
  struct Node* nxt;
  size_t size;//存有效数据大小，含结构体 
  uintptr_t length;//整段长度，含未分配的
}alloc_node,*palloc_node;
palloc_node a_head, a_tail;

static void pmm_init() {
  pm_start = (uintptr_t)_heap.start;
  pm_end   = (uintptr_t)_heap.end;
  //my
  a_head = a_tail = (palloc_node)pm_start;
  a_head->nxt = NULL;
  a_head->length = pm_end-pm_start;
  a_head->size = MIN_LEN;
}

static void *kalloc(size_t size) {
  //my
  alloc_lock();
  assert(size>=0);
  palloc_node ret = NULL;
  uint32_t valid_len = a_tail->length - a_tail->size;
  size_t new_size = size+MIN_LEN;
  if(valid_len >= new_size){
    //插入尾部
    ret = a_tail->nxt = NEXT(a_tail);
    ret->length = valid_len;
    ret->size = new_size;
    a_tail->length=a_tail->size;
    a_tail = ret;
  }else{
    /*ret = a_head;
    while(ret!=NULL){
      
    }*/
  }
  alloc_unlock();
  return ret;
}

static void kfree(void *ptr) {
}

MODULE_DEF(pmm) {
  .init = pmm_init,
  .alloc = kalloc,
  .free = kfree,
};
