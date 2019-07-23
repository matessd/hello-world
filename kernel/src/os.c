#include <my_os.h>
#include <my_test.h>
void idle(){
  while(1){
    _yield();
  }
}
static void os_init() {
  pmm->init();
  kmt->init();
  dev->init();
  //vfs->init();
  //print是线程名，tty是用来查dev的参数
  kmt->create(pmm->alloc(sizeof(task_t)), "print1", echo_task, "tty1");
  kmt->create(pmm->alloc(sizeof(task_t)), "print2", echo_task, "tty2");
  kmt->create(pmm->alloc(sizeof(task_t)), "print3", echo_task, "tty3");
  kmt->create(pmm->alloc(sizeof(task_t)), "print4", echo_task, "tty4");
}

static void hello() {
  for (const char *ptr = "Hello from CPU #"; *ptr; ptr++) {
    _putc(*ptr);
  }
  _putc("12345678"[_cpu()]); _putc('\n');
  //printf("%d**\n",_cpu());//my
}

static void os_run() {
  hello();
  _intr_write(1);
  //my_pmmtest();//my
  while (1) {
    _yield();
  }
  assert(0);
}

irq_t handlers[10];//中断处理程序
volatile int n_handler = 0;

static _Context *os_trap(_Event ev, _Context *context) {
  _Context *ret = NULL;
  for(int i=0; i<n_handler; i++) {
    if (handlers[i].ev == _EVENT_NULL || handlers[i].ev == ev.event) {
      _Context *next = handlers[i].handler(ev, context);
      if (next) ret = next;
    }
  }
  return ret;
}

static void os_on_irq(int seq, int event, handler_t handler) {
  //printf("1\n");
  int cur = 0;
  for(int i=0; i<n_handler; i++){
    if(seq>handlers[i].seq)
      cur++;
    else break;
  }
  irq_t hand = (irq_t){seq,event,handler};
  if(cur==n_handler)
    handlers[cur] = hand;
  else{
    irq_t tmp;
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
