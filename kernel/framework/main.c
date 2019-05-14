#include <kernel.h>
#include <klib.h>

int main() {
  _ioe_init();
  _cte_init(os->trap);

  // call sequential init code
  os->init();
  printf("%d\n",(int)(uint32_t)_heap.start);
  printf("%d\n",(int)(uint32_t)_heap.end);
  _mpe_init(os->run); // all cores call os->run()
  return 1;
}
