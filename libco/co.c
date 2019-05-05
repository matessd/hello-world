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
    char stack[4096*1024];
    jmp_buf buf;
};

struct co *current = NULL;
struct co *coroutines[5];
int g_cnt = 0;
void *__stack_backup;
intptr_t __stack;
func_t g_func;
void *g_arg;

void co_init() {
    for(int i=0; i<5; i++)
        coroutines[i] = NULL;
    current = coroutines[0] = (struct co*)malloc(sizeof(struct co));//泄漏？
    assert(current);
}

struct co* co_start(const char *name, func_t func, void *arg) {
  struct co* new = (struct co *)malloc(sizeof(struct co));
  assert(new);
  current = new;
  for(int i=1; i<5; i++){
      if(coroutines[i]==NULL){
          coroutines[i] = current;
          break;
      }
  }
  //assert(0);
  if(setjmp(coroutines[0]->buf)==0){
      __stack = (intptr_t)current->stack;
      //printf("%x\n",(int)(intptr_t)current->stack);
      __stack -= __stack%16;
      __stack += sizeof(current->stack);//这一步如果按照讲义那样，汇编时会先赋值给sp，再加
      //printf("%x\n",(int)(intptr_t)__stack);
      //printf("%x\n",(int)__stack);
      g_func = func;
      g_arg = arg;
      current->if_run = 1;
      asm volatile("mov " SP ", %0; mov %1, " SP :
                   "=g"(__stack_backup) :
                   "g"(__stack));
      g_func(g_arg);
      current->if_run = 0;
      asm volatile("mov %0," SP : : "g"(__stack_backup));
  }
  //func(arg); // Test #2 hangs
  else{
      /*asm volatile("mov %0," SP : : "g"(__stack_backup));*/
      current = coroutines[0];
  } 
  return new;
}

void co_yield() {
  //assert(0);
  if(setjmp(current->buf)==0){
       for(int i=0; i<5; i++){
           g_cnt = (g_cnt+1)%5;
           if(coroutines[g_cnt]!=NULL)
               break;
       }
       //printf("%d\n",g_cnt);
       current = coroutines[g_cnt];
       longjmp(current->buf,1);
  }else{
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

