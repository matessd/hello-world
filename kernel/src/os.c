#include <common.h>
#include <klib.h>
#include <my_os.h>
#include <my_test.h>

static void os_init() {
  pmm->init();
}

//LOCKDEF(os) //my
static void hello() {
  for (const char *ptr = "Hello from CPU #"; *ptr; ptr++) {
    _putc(*ptr);
  }
  _putc("12345678"[_cpu()]); _putc('\n');
  //printf("%d**\n",_cpu());//my
}

static void os_run() {
  kmt->spin_lock(os_lk);//my
  hello();
  kmt->spin_unlock(os_lk);//my
  //assert(a_head->nxt==NULL);
  _intr_write(1);
  //pmm_test();//my
  while (1) {
    _yield();
  }
  assert(0);
}

static _Context *os_trap(_Event ev, _Context *context) {
  return context;
}

static void os_on_irq(int seq, int event, handler_t handler) {
}

MODULE_DEF(os) {
  .init   = os_init,
  .run    = os_run,
  .trap   = os_trap,
  .on_irq = os_on_irq,
};
