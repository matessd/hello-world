#include <my_os.h>
static uintptr_t pm_start, pm_end;

//my
#define MIN_LEN 20
//#define INIT_VALUE 0xcccccccc
spinlock_t Alloc_lk;
spinlock_t *alloc_lk=&Alloc_lk;
int memtot, memfree;

static void pmm_init() {
  pm_start = (uintptr_t)_heap.start;
  pm_end   = (uintptr_t)_heap.end;
  
  memtot = (int)(uintptr_t)_heap.end - (int)(uintptr_t)_heap.start;
  memfree = memtot;

  //my
  a_head = (palloc_t)pm_start;
  a_head->nxt = NULL;
  a_head->prev = NULL;
  a_head->st = ((uintptr_t)a_head)+MIN_LEN;
  a_head->ed = pm_end;//不可用
  a_head->fence = FENCE;
  //my spin_lock
  for(int i=0; i<8; i++)
    cpu_cli[i].ncli = 0;
  //kmt->spin_init(os_lk, "os_lk");
  kmt->spin_init(alloc_lk, "alloc_lk");
  kmt->spin_init(task_lk, "task_lk");
}

static void *kalloc(size_t size) {
  //my
  kmt->spin_lock(alloc_lk);
  //assert(size>=0);
  if(size==0) return NULL;
  palloc_t ret = NULL;
  palloc_t cur = a_head;
  size_t new_size = size+MIN_LEN;
  while(cur!=NULL){
    //assert(a_head->ed>=a_head->st);
    if(cur->ed - cur->st >=new_size){
      ret = (palloc_t)(cur->ed - new_size);
      ret->st = ret->ed = cur->ed;
      cur->ed = (uintptr_t)ret;

      palloc_t nxt = cur->nxt;
      ret->nxt = nxt;
      if(nxt!=NULL) nxt->prev = ret;

      ret->prev = cur;
      cur->nxt = ret;
      break;
    }
    cur = cur->nxt;
  }
  if(ret==NULL) return NULL;

  //vfs
  char src[128], ctmp[128];
  sprintf(src,"MemTotal:  %d B\n",memtot);
  strcpy(ctmp, src);
  memfree -= new_size;
  sprintf(src,"MemFree:   %d B\n",memfree);
  strcat(ctmp, src);
  vfs->write("/proc/meminfo", ctmp, 0);

  //清零，为了tty和input.c
  memset(ret+1,0,size);

  ret->fence = FENCE;
  kmt->spin_unlock(alloc_lk);
  return ret+1;
}

static void kfree(void *ptr) {
  if(ptr==NULL) return;
  kmt->spin_lock(alloc_lk);
  palloc_t cur = ((palloc_t)ptr)-1;
  assert(cur->fence == FENCE);//保证free有效性
  cur->fence = 0;

  //vfs
  char src[128], ctmp[128];
  sprintf(src,"MemTotal:  %d B\n",memtot);
  strcpy(ctmp, src);
  memfree += cur->ed - (int)(intptr_t)cur;
  sprintf(src,"MemFree:   %d B\n",memfree);
  strcat(ctmp, src);
  vfs->write("/proc/meminfo", ctmp, 0);

  if(cur==a_head){
    cur->st = ((uintptr_t)cur)+MIN_LEN;
    return;
  }
  palloc_t prev = cur->prev;
  palloc_t nxt = cur->nxt;
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
