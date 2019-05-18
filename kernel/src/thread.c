#include<my_os.h>
volatile int ntask;
spinlock_t Task_lk;
spinlock_t *task_lk = &Task_lk;
int kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg){
  //默认在中断打开前create
  _kcontext((_Area){task,task+1}, entry, arg);
  task->name = name;
  //cpu个数一开始就不是0

  //task->id = ntask;
  task->nxt = NULL;
  task->sleep_flg = 0;
  task->fence = FENCE;

  kmt->spin_lock(task_lk);
  int cnt = ntask%_ncpu();
  //printf("%d**\n",ntask);
  task->id = ntask++;
  //如果多处理器好了这样会不会有问题？
  add_head(task,cnt);
  kmt->spin_unlock(task_lk);

  if(_intr_read()){
    _yield();
  }
  return 0;
}

void teardown(task_t *task){
  return;
}

void add_tail(task_t *task){
  //kmt->spin_lock(task_lk);
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
  //kmt->spin_unlock(task_lk);
}

void add_head(task_t *task, int cnt){
  //printf("%s\n",task->name); 
  //kmt->spin_lock(task_lk); 
  task_t *cur = Task_head[cnt];
  assert(cur!=task);
  if(cur==NULL){
    //printf("")
    Task_head[cnt] = task;
    task->nxt = NULL;
    return;
  }
  //task_t *tmp_head = Task_head
  Task_head[cnt] = task;
  task->nxt = cur;
  //kmt->spin_unlock(task_lk);
}

void del_head(){
  //kmt->spin_lock(task_lk);
  //task_t *tmp_head = task_head;
  task_t *nxt = task_head->nxt;
  task_head->nxt = NULL;
  task_head = nxt;
  //kmt->spin_unlock(task_lk);
  //return tmp_head;
}

_Context *kmt_context_save(_Event ev, _Context *context){
  //assert(current!=NULL);
  printf("**cur: %s\n",current->name);
  printf("cpu: %d\n",_cpu());
  if(current) {
    //assert(current->fence == FENCE);
    current->context = *context;
      /*task_t *tmp = task_head;
        assert(tmp!=current);
        assert(tmp!=tmp->nxt);
        }
        assert(tmp!=current);*/
    //在sem睡眠队列中
    if(current->sleep_flg==0){
      //kmt->spin_lock(task_lk);
      add_tail(current);
      //kmt->spin_unlock(task_lk);
    }
  }
  return NULL;
}

_Context *kmt_context_switch(_Event ev, _Context *context){ 
  //assert(task_head!=NULL);
  printf("current: %s\n",task_head->name);

  //kmt->spin_lock(task_lk);
  current = task_head;
  assert(task_head!=NULL);
  del_head(); 
  //kmt->spin_unlock(task_lk);

  return &current->context;
}
