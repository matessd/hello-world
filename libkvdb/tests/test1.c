#include "../kvdb.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
volatile int cnt;

void *test1(void *_db) {
  kvdb_t *db = _db;
  assert(kvdb_open(db, "b.db")==0);
  char key[20],v[20];
  int i = 0;
  srand(time(NULL));
  while(i++<10000){
    sprintf(key,"**%d**\0",++cnt);
    strcpy(v,key);
    //strcat(v,key);
    //strcat(v,key);
    //printf("%s\n",v);
    assert(kvdb_put(db, key, v)==0);
    char *value = kvdb_get(db,key);
    assert(value!=NULL);
    printf("[key:%s][value:%s]\n",key,value);
    //assert(key,value)==0;
    free(value);
  } 
  return NULL;
}

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
    assert(kvdb_close(g_db[i])==0);
    free(g_db[i]);
  }
  return 0;
}
