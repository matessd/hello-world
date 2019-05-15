#include<klib.h>
#include<common.h>
#include<my_os.h>
LOCKDEF(test)
char *ptr[4096];
void pmm_test(){
  srand(0);
  //size_t size;
  test_lock();
  for(int i=0; i<4096;i++){
    ptr[i] = pmm->alloc(200);
    if(ptr[i]==NULL){
      printf("fail on %d\n",i);
      return;
    }
    //test_lock();
    sprintf(ptr[i], "hello%d\n",i);
    printf("%s\n",ptr[i]);
    //test_unlock();
  }
  for(int i=0; i<4096; i++)
    printf("%s\n",ptr[i]);
  test_unlock();
}
