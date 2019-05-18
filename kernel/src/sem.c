#include<my_os.h>
void sem_init(sem_t *sem, const char *name, int value){
  sem->value = value;
  kmt->spin_init(&sem->lk, name);
  sem->end = sem->start = 0;
  return;
}
void sem_wait(sem_t *sem){
  kmt->spin_lock(&sem->lk);
  sem->value--;
  if (sem->value < 0) {
    sem->queue[sem->end] = current;
    sem->end = (sem->end + 1) % NPROC;
    //先解锁？
    kmt->spin_unlock(&sem->lk);
    _yield();
    kmt->spin_lock(&sem->lk);
  }
  kmt->spin_unlock(&sem->lk);
  return;
}
void sem_signal(sem_t *sem){
  kmt->spin_lock(&sem->lk);
  sem->value++;
  int wake_flg = 0;
  if (sem->value <= 0) {
    wake_flg = 1;
    add_head(sem->queue[sem->start], _cpu());
    //wakeup(s->queue[s->start]);
    sem->queue[sem->start] = NULL;
    sem->start = (sem->start + 1) % NPROC;
  }
  kmt->spin_unlock(&sem->lk);
  if(wake_flg) _yield();
  return;
}
