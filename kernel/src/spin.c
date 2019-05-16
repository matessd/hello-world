#include<common.h>
#include<klib.h>
#include<my_os.h>
void spin_init(spinlock_t *lk, const char *name){  
  lk->name = name;
  lk->locked = 0;
  lk->cpu = 0;
}

void spin_lock(spinlock_t *lk){
  //pushcli(); // disable interrupts to avoid deadlock.
  //if(holding(lk))
    //panic("acquire");

  // The xchg is atomic.
  while(_atomic_xchg(&lk->locked, 1) != 0);

  // Tell the C compiler and the processor to not move loads or stores
  // past this point, to ensure that the critical section's memory
  // references happen after the lock is acquired.
  //__sync_synchronize();

  // Record info about lock acquisition for debugging.
  lk->cpu = _cpu();
}

void spin_unlock(spinlock_t *lk){
  return;
}
  
MODULE_DEF(kmt) {
  .spin_init = spin_init,
  .spin_lock = spin_lock,
  .spin_unlock = spin_unlock,
};
