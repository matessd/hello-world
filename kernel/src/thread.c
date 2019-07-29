#include<my_os.h>
volatile int Ntask;
spinlock_t Task_lk;
spinlock_t *task_lk = &Task_lk;
int kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg){
  _kcontext((_Area){task,task+1}, entry, arg);
  task->name = name;
  //cpu个数一开始就不是0

  //task->id = ntask;
  task->nxt = NULL;
  task->sleep_flg = 0;
  task->fence = FENCE;

  kmt->spin_lock(task_lk);
  task->id = Ntask+1;

  //vfs 
  char src[64]; char path[64]; 
  src[0]='\0'; path[0]='\0';
  sprintf(path,"/proc/%d",Ntask+1);
  vfs->mkdir(path, 0, 1);
  sprintf(path, "/proc/%d/status", Ntask+1);
  vfs->mkdir(path, 1, 1);
  sprintf(src, "Name: %s\nPid: %d\n",name,Ntask+1);
  //printf("Name: %s\nPid: %d\n",name,Ntask+1);
  vfs->write(path, src, 0);

  int cnt = Ntask++%_ncpu();
  //如果多处理器准备好了这样会不会有问题？
  //add_head(task,cnt);
  //current = task;
  //ntask[cnt]++;
  tasks[cnt][cpu_ntask[cnt]++] = task;
  kmt->spin_unlock(task_lk);

  if(_intr_read()){
    _yield();
  }
  return 0;
}

void teardown(task_t *task){
  return;
}

/*void add_tail(task_t *task){
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
}*/

_Context *kmt_context_save(_Event ev, _Context *context){
  //assert(current!=NULL);
  //printf("**cur: %s\n",current->name);
  //printf("cpu: %d\n",_cpu());
  //assert(current->fence)
  if(current) {
    assert(current->fence == FENCE);
    current->context = *context;

  }
  return NULL;
}

_Context *kmt_context_switch(_Event ev, _Context *context){ 
  //assert(task_head!=NULL);

  //kmt->spin_lock(task_lk);
  int i=Curr[_cpu()];
  char ctmp[256], src[128]; ctmp[0] = '\0';
  while(1){
    //printf("%d\n",i);
    i=(i+1)%ntask;
    task_t *task = tasks[_cpu()][i];
    if(task->sleep_flg==0){
      task->swcnt++;
      sprintf(src,"Name: %s\nPid: %d\n",task->name,task->id);
      strcat(ctmp, src);
      sprintf(src,"ctxt_switches: %d\n",task->swcnt);
      strcat(ctmp, src);
      sprintf(src,"/proc/%d/status",task->id);
      vfs->write(src, ctmp, 1);
      Curr[_cpu()] = i;
      break;
    }
  }
  assert(current!=NULL);
  //current = task_head;
  //assert(task_head!=NULL);
  //del_head(); 
  //kmt->spin_unlock(task_lk);

  return &current->context;
}
