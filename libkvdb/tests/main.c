#include "../kvdb.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
volatile int cnt;

void *test(void *_db) {
  kvdb_t *db = _db;
  char key[20];
  while(1){
    sprintf(key,"%d\0",++cnt);
    assert(kvdb_put(db, key, key)==0);
  } 
  return NULL;
}

#define THREADS 4

int main(int argc, char *argv[]) {
  kvdb_t *db = malloc(sizeof(kvdb_t));
  assert(db != NULL);

  assert(kvdb_open(db, argv[1])==0);

  pthread_t pt[THREADS];
  for(int i = 0; i < THREADS; i++) {
    pthread_create(&pt[i], NULL, test, db);

  }
  for(int i = 0; i < THREADS; i++) {
    pthread_join(pt[i], NULL);
  }
  assert(kvdb_close(db)==0);
  free(db);
  return 0;
}
