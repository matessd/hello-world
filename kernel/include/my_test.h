#include<klib.h>
#include<common.h>
#include<my_os.h>
LOCKDEF(test)
char *ptr[1024];
int i=0;
void pmm_test(){
  srand(0);
  //size_t size;
  while(i<1024){
    ptr[i] = pmm->alloc(1000);
    if(ptr[i]==NULL){
      assert(0);
      return;
    }
    test_lock();
    sprintf(ptr[i], "hello%d\n",i);
    printf("%s\n",ptr[i]);
    test_unlock();
    i++;
  }
  while(i>0){
    printf("%s\n",ptr[--i]);
    pmm->free(ptr[i]);
  }
  assert(a_head->nxt==NULL);
}
