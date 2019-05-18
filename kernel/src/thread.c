#include<my_os.h>
volatile int ntask = 0;
spinlock_t Create_lk;
spinlock_t *create_lk = &Create_lk;
int kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg){
  //默认在中断打开前create
  _kcontext((_Area){task,task+1}, entry, arg);
  task->name = name;
  //cpu个数一开始就不是0
  //预防多处理器

  kmt->spin_lock(create_lk);
  task->id = ntask;
  task->nxt = NULL;
  task->sleep_flg = 0;
  task->fence = FENCE;
  int i = ntask++ %_ncpu();
  printf("%d^^\n",i)
  kmt->spin_unlock(create_lk);

  add_head(task,i);
  //printf("%d\n",_intr_read());
  if(_intr_read()){
    //assert(0);
    _yield();
  }
  return 0;
}

void teardown(task_t *task){
  return;
}

void add_tail(task_t *task){
  task_t *cur = task_head;
  if(task_head==NULL){
    task_head = task;
    task->nxt = NULL;
    return;
  }
  while(cur->nxt){
    //printf("%s\n",cur->nxt->name);
    assert(cur!=cur->nxt);
    cur = cur->nxt;
  }
  cur->nxt = task;
  task->nxt = NULL;
}

void add_head(task_t *task, int i){
  task_t *cur = Task_head[i];
  assert(cur!=task);
  if(cur==NULL){
    Task_head[i] = task;
    task->nxt = NULL;
    return;
  }
  //task_t *tmp_head = Task_head
  Task_head[i] = task;
  task->nxt = cur;
}

void del_head(){
  task_t *nxt = task_head->nxt;
  task_head->nxt = NULL;
  task_head = nxt;
}

_Context *kmt_context_save(_Event ev, _Context *context){
  //Can current be NULL?
  //assert(current!=NULL);
  //if(current==NULL)
  printf("**cur: %s\n",current->name);
  if(current) {
    assert(current->fence == FENCE);
    current->context = *context;
      /*task_t *tmp = task_head;
        while(tmp->nxt!=NULL){
        assert(tmp!=current);
        assert(tmp!=tmp->nxt);
        }
        assert(tmp!=current);*/
    //在sem睡眠队列中
    if(current->sleep_flg==0){
      add_tail(current);
    }
  }
  //printf("%d&&&&\n",_cpu());
  return NULL;
}

_Context *kmt_context_switch(_Event ev, _Context *context){ 
  assert(task_head!=NULL);
  //printf("current: %s\n",task_head->name);
  //if(task_head==NULL) return context;
  current = task_head;
  del_head(); 
  return &current->context;
}
