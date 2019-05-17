#include <kernel.h>
#include <klib.h>

int main() {
  _ioe_init();
  _cte_init(os->trap);

  // call sequential init code
  os->init();
  printf("%d\n",_ncpu());
  return 0;
  _mpe_init(os->run); // all cores call os->run()
  return 1;
}
