#include<my_os.h>
int kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg){
   
  return 0;
}

void teardown(task_t *task){
  return;
}

_Context *kmt_context_save(_Event ev, _Context *context){
  return *context;
}

_Context *kmt_context_switch(_Event ev, _Context *context){
  
  return *context;
}
