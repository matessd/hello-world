#include <my_os.h>
#include <my_test.h>

static void os_init() {
  pmm->init();
  kmt->init();
  dev->init();
  //kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty1");
  kmt->create(pmm->alloc(sizeof(task_t)), "idle",idle, NULL);
}

//LOCKDEF(os) //my
static void hello() {
  for (const char *ptr = "Hello from CPU #"; *ptr; ptr++) {
    _putc(*ptr);
  }
  _putc("12345678"[_cpu()]); _putc('\n');
  //printf("%d**\n",_cpu());//my
}

//spinlock_t Os_lk;
//spinlock_t *os_lk=&Os_lk;

static void os_run() {
  //kmt->spin_lock(os_lk);//my
  hello();
  //kmt->spin_unlock(os_lk);//my
  //assert(a_head->nxt==NULL);
  _intr_write(1);
  //pmm_test();//my
  while (1) {
    _yield();
  }
  assert(0);
}

irq_handler handlers[10];//中断处理程序
volatile int n_handler = 0;

static _Context *os_trap(_Event ev, _Context *context) {
  //printf("%d\n",ntask);
  //assert(ev.event!=_EVENT_NULL);  
  _Context *ret = NULL;
  for(int i=0; i<n_handler; i++) {
    if (handlers[i].ev == _EVENT_NULL || handlers[i].ev == ev.event) {
      //printf("%d\n",i);
      _Context *next = handlers[i].handler(ev, context);
      //printf("%d**\n",i);
      if (next) ret = next;
    }
  }
  return ret;
  //return context;
}

static void os_on_irq(int seq, int event, handler_t handler) {
  //printf("1\n");
  int cur = 0;
  for(int i=0; i<n_handler; i++){
    if(seq>handlers[i].seq)
      cur++;
    else break;
  }
  irq_handler hand = (irq_handler){seq,event,handler};
  if(cur==n_handler)
    handlers[cur] = hand;
  else{
    irq_handler tmp;
    while(cur<n_handler){
      tmp = handlers[cur];
      handlers[cur++] = hand;
    }
    handlers[cur] = tmp;
  }
  n_handler++;
}

MODULE_DEF(os) {
  .init   = os_init,
  .run    = os_run,
  .trap   = os_trap,
  .on_irq = os_on_irq,
};
