#include<klib.h>
#include<common.h>
void pmm_test(){
  srand(0);
  char* ptr[4096];
  //size_t size;
  for(int i=0; i<4096;++){
    ptr[i] = kalloc(200);
    if(ptr[i]==NULL){
      printf("fail on %d\n",i);
      return;
    }
    sprintf(ptr[i], "hello%d\n",i);
  }
  for(int i=0; i<4096; i++)
    printf("%s\n",ptr[i]);
}