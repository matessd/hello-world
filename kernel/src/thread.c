#include<my_os.h>
int kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg){
  _kcontext((_Area){task,task+1}, entry, arg);
  task->name = name;
  //cpu个数一开始是0吗？
  if(_ncpu()==0)
  add_head(task);
  if(_intr_read()){
    yield();
  }
  return 0;
}

void teardown(task_t *task){
  return;
}

void add_tail(task_t *task){
  task_t *cur = task_head;
  if(cur==NULL){
    task_head = task;
    task->nxt = NULL;
    return;
  }
  while(cur->nxt){
    cur = cur->nxt;
  }
  cur->nxt = task;
}

void add_head(task_t *task){
  task_t *cur = task_head;
  if(cur==NULL){
    task_head = task;
    task->nxt = NULL;
    return;
  }
  task_head = task;
  task->nxt = cur;
}

void del_head(){
  task_head = task_head->nxt;
}

_Context *kmt_context_save(_Event ev, _Context *context){
  //Can current be NULL?
  //assert(current!=NULL);
  if(current) {
    current->context = *context;
    add_tail(current);
  }
  return context;
}

_Context *kmt_context_switch(_Event ev, _Context *context){ 
  //assert(task_head!=NULL);
  if(task_head==NULL) return context;
  current = task_head;
  del_head(); 
  return &current->context;
}
