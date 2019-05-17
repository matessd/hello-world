#include<my_os.h>
int kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg){
   
  return 0;
}

void teardown(task_t *task){
  return;
}

_Context *kmt_context_save(_Event ev, _Context *context){
  //Can current be NULL?
  if(!current) current->context = *context;
  return context;
}

_Context *kmt_context_switch(_Event ev, _Context *context){  
  while((current - tasks) % _ncpu() != _cpu()){
    if (!current || current + 1 == &tasks[LENGTH(tasks)]) {
      current = &tasks[0];
    } else {
      current++;
    }
  }
  //printf("\n[cpu-%d] Schedule: %s\n", _cpu(), current->name);
  return &current->context;
}
