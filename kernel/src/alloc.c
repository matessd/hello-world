#include <common.h>
#include <klib.h>

static uintptr_t pm_start, pm_end;

//my
typedef struct{
  uintptr_t pos; size_t size;
}alloc_node,*palloc_node;
//palloc_node a_lloc_

static void pmm_init() {
  pm_start = (uintptr_t)_heap.start;
  pm_end   = (uintptr_t)_heap.end;
}

static void *kalloc(size_t size) {
  return NULL;
}

static void kfree(void *ptr) {
}

MODULE_DEF(pmm) {
  .init = pmm_init,
  .alloc = kalloc,
  .free = kfree,
};
