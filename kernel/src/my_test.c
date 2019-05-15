#include<klib.h>
#include<common.h>
#include<my_os.h>
LOCKDEF(test)
volatile char *ptr[100];
volatile int i=0;
void pmm_test(){
  srand(0);
  //size_t size;
  while(i<100){
    test_lock();
    ptr[i] = pmm->alloc(1000);
    if(ptr[i]==NULL){
      assert(0);
      return;
    }
    sprintf(ptr[i], "hello%d\n",i);
    printf("%s\n",ptr[i]);
    i++;
    test_unlock();
  }
  while(i>0){
    printf("%s\n",ptr[--i]);
    pmm->free(ptr[i]);
  }
  assert(a_head->nxt==NULL);
}
