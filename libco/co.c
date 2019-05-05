#include <stdio.h>
#include "co.h"
#include <setjmp.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#if defined(__i386__)
    #define SP "%%esp"
#elif defined(__x86_64__)
    #define SP "%%rsp"
#endif

struct co {
    int if_run;
    char stack[4096];
    jmp_buf buf;
};

struct co *current = NULL;
struct co *coroutines[5];
int g_cnt = 0;
void *__stack_backup[5];

void co_init() {
    for(int i=0; i<5; i++)
        coroutines[i] = NULL;
    current = coroutines[0] = (struct co*)malloc(sizeof(struct co));//泄漏？
    assert(current);
}

struct co* co_start(const char *name, func_t func, void *arg) {
  struct co* new = (struct co *)malloc(sizeof(struct co));
  //assert(new);
  current = new;
  //for(int i=1; i<5; i++){
      //if(coroutines[i]==NULL){
          //coroutines[1] = current;
          //break;
      //}
  //}
  //assert(0);
  if(setjmp(coroutines[0]->buf)==0){
      //printf("%s\n",name);
      //intptr_t tst = current->stack;
      //printf("%x\n",(int)(intptr_t)current->stack);
      //intptr_t stack = (intptr_t)current->stack;
      //stack -= stack%16;
      //printf("%x\n",(int)stack);
      /*asm volatile("mov " SP ", %0; mov %1, " SP :
                   "=g"(__stack_backup[0]) :
                   "g"(stack + sizeof(current->stack)));*/
      //assert(0);
      func(arg);
      /*asm volatile("mov %0," SP : : "g"(__stack_backup[0]));*/
  }
  //func(arg); // Test #2 hangs
  else{
      /*asm volatile("mov %0," SP : : "g"(__stack_backup[0]));*/
      current = coroutines[0];
  } 
  return new;
}

void co_yield() {
  //assert(0);
  if(setjmp(current->buf)==0){
       current->if_run = 1;
       for(int i=0; i<5; i++){
           g_cnt = (g_cnt+1)%5;
           if(coroutines[g_cnt]!=NULL)
               break;
       }
       //printf("%d\n",g_cnt);
       current = coroutines[g_cnt];
       longjmp(current->buf,1);
  }else{
       current->if_run = 0;
       return;
  }
}

void co_wait(struct co *thd) {
  current = coroutines[0];
  while(thd->if_run){
      if(setjmp(current->buf)==0){
          current = thd;
          longjmp(current->buf,1);
      }else{
          current = coroutines[0];
      }
  }
  free(thd);
  for(int i=1; i<5; i++){
      if(coroutines[i]==thd){
          coroutines[i]=NULL;
          break;
      }
  }
}

