#include "../kvdb.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
volatile int cnt;


#define THREADS 4

kvdb_t *g_db[THREADS];
int main(int argc, char *argv[]) {
  for(int i=0; i<THREADS; i++){
    g_db[i] = malloc(sizeof(kvdb_t));
    assert(g_db[i] != NULL);
  }

  //assert(kvdb_open(db, "b.db")==0);
  pthread_t pt[THREADS];
  for(int i = 0; i < THREADS; i++) {
    pthread_create(&pt[i], NULL, test1, g_db[i]);
  }
  for(int i = 0; i < THREADS; i++) {
    pthread_join(pt[i], NULL);
  }
  for(int i=0; i<THREADS; i++){
    assert(kvdb_close(g_db)==0);
    free(g_db[i]);
  }
  return 0;
}
