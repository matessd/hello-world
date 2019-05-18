#include<my_os.h>
/*typedef struct Cpu_cli{
  int ncli, intena;
}cpu_cli[8];*/
volatile cli_sta cpu_cli[8];
volatile cli_sta* mycpu(){
  return &cpu_cli[_cpu()];
}
int holding(spinlock_t *lk);
void pushcli(void);
void popcli(void);

void spin_init(spinlock_t *lk, const char *name){  
  lk->name = name;
  lk->locked = 0;
  lk->cpu = 0;
}

void spin_lock(spinlock_t *lk){
  //printf("lock: %s %d\n",lk->name,_cpu());
  printf("lock: %s %d; cpu: %d\n",lk->name, lk->locked,lk->cpu);
  pushcli(); // disable interrupts to avoid deadlock.
  if(holding(lk)){
    //dpanic("acquire");
    printf("acquire: %s %d\n",lk->name,lk->cpu);
    //_halt(1);
  }

  // The xchg is atomic.
  while(_atomic_xchg(&lk->locked, 1) != 0);

  // Tell the C compiler and the processor to not move loads or stores
  // past this point, to ensure that the critical section's memory
  // references happen after the lock is acquired.
  //__sync_synchronize();
  asm volatile("": : :"memory");
    
  // Record info about lock acquisition for debugging.
  lk->cpu = _cpu();
}

void spin_unlock(spinlock_t *lk){
  printf("unlock: %s %d\n",lk->name,_cpu());
  if(!holding(lk))
    dpanic("release");
  lk->cpu = 0;

  // Tell the C compiler and the processor to not move loads or stores
  // past this point, to ensure that all the stores in the critical
  // section are visible to other cores before the lock is released.
  // Both the C compiler and the hardware may re-order loads and
  // stores; __sync_synchronize() tells them both not to.
  //__sync_synchronize();
  asm volatile("": : :"memory");

  // Release the lock, equivalent to lk->locked = 0.
  // This code can't use a C assignment, since it might
  // not be atomic. A real OS would use C atomics here.
  //asm volatile("movl $0, %0" : "+m" (lk->locked) : );
  //asm volatile("movl $0, %0" : "+m" (lk->locked) : );
  _atomic_xchg(&lk->locked, 0);

  popcli();
  return;
}
 
// Check whether this cpu is holding the lock.
int holding(spinlock_t *lk){
  int r;
  pushcli();
  r = lk->locked && lk->cpu == _cpu();
  popcli();
  return r;
} 

// Pushcli/popcli are like cli/sti except that they are matched:
// it takes two popcli to undo two pushcli.  Also, if interrupts
// are off, then pushcli, popcli leaves them off.

void pushcli(void){
  uint32_t intena = _intr_read();
  _intr_write(0); //cli
  if(mycpu()->ncli == 0)
    mycpu()->intena = intena;
  mycpu()->ncli += 1;
}

void popcli(void){
  if(_intr_read())
    dpanic("popcli - interruptible");
  if(--mycpu()->ncli < 0)
    dpanic("popcli");
  if(mycpu()->ncli == 0 && mycpu()->intena)
    _intr_write(1);
}
