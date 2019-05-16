#include<klib.h>
#include<my_os.h>

void kmt_init(){
  return;
}
int kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg){
  return 0;
}

void teardown(task_t *task){
  return;
}

void spin_init(spinlock_t *lk, const char *name);
void spin_lock(spinlock_t *lk);
void spin_unlock(spinlock_t *lk);

void sem_init(sem_t *sem, const char *name, int value){
  return;
}
void sem_wait(sem_t *sem){
  return;
}
void sem_signal(sem_t *sem){
  return;
}
  
MODULE_DEF(kmt) {
  .init = kmt_init,
  .create = kmt_create,
  .teardown = teardown,
  .sem_init = sem_init,
  .sem_wait = sem_wait,
  .sem_signal = sem_signal,
  .spin_init = spin_init,
  .spin_lock = spin_lock,
  .spin_unlock = spin_unlock,
};
