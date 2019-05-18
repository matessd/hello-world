#include<my_os.h>
//LOCKDEF(test)
char *ptr[100];
volatile int i=0;
void pmm_test(){
  //srand(0);
  //size_t size;
  //test_lock();
  while(i<100){
    //test_lock();
    ptr[i] = pmm->alloc(1000);
    if(ptr[i]==NULL){
      assert(0);
      return;
    }
    sprintf(ptr[i], "hello%d\n",i);
    printf("%d\n",i);
    i++;
    //test_unlock();
  }
  while(i>0){
    printf("%s\n",ptr[--i]);
    pmm->free(ptr[i]);
  }
  assert(a_head->nxt==NULL);
  //test_unlock();
}

void echo_task(void *name) {
  device_t *tty = dev_lookup(name);
  while (1) {
    char line[128], text[128];
    sprintf(text, "(%s) $ ", name); tty_write(tty, text);
    int nread = tty->ops->read(tty, 0, line, sizeof(line));
    line[nread - 1] = '\0';
    sprintf(text, "Echo: %s.\n", line); tty_write(tty, text);
  }
}
