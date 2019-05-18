#include<my_os.h>
void sem_init(sem_t *sem, const char *name, int value){
  sem->value = value;
  kmt->spin_init(&sem->lk1, name);
  kmt->spin_init(&sem->lk2, name);
  sem->end = sem->start = 0;
  return;
}
void sem_wait(sem_t *sem){
  kmt->spin_lock(&sem->lk1);
  sem->value--;
  if (sem->value < 0) {
    assert(task_head!=NULL);
    current->sleep_flg = 1;
    sem->queue[sem->end] = current;
    sem->end = (sem->end + 1) % NPROC;
    //先解锁？
    //kmt->spin_unlock(&sem->lk);
    _yield();
    //kmt->spin_lock(&sem->lk);
  }
  kmt->spin_unlock(&sem->lk1);
  return;
}
void sem_signal(sem_t *sem){
  kmt->spin_lock(&sem->lk2);
  sem->value++;
  int yield_flg = 0;
  assert(sem->value>=0);
  if (sem->value <= 0) {
    sem->queue[sem->start]->sleep_flg = 0;
    yield_flg = 1;
    add_head(sem->queue[sem->start], _cpu());
    //wakeup(s->queue[s->start]);
    sem->queue[sem->start] = NULL;
    sem->start = (sem->start + 1) % NPROC;
  }
  //kmt->spin_unlock(&sem->lk);
  if(yield_flg) _yield();
  kmt->spin_unlock(&sem->lk2);
  return;
}
